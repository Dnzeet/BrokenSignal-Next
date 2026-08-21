#include "AudioFileSourceHTTPSStream.h"
#include "Radio.h"
#include "Browser.h"
#include "UI.h"

extern bool radioForceAac;

#if DEBUG_SERIAL
#define RDBG(...) Serial.printf(__VA_ARGS__)
#else
#define RDBG(...) ((void)0)
#endif

void loadRadioList()
{
    radioCount = 0;
    File f = SD.open("/Music/_radio/webradio.cfg", FILE_READ);
    if (!f)
        return;
    while (f.available() && radioCount < RADIO_MAX)
    {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length() == 0)
            continue;
        int sep = line.indexOf('|');
        if (sep < 0)
        {
            radioList[radioCount].name = "Radio " + String(radioCount + 1);
            radioList[radioCount].url = line;
        }
        else
        {
            radioList[radioCount].name = line.substring(0, sep);
            radioList[radioCount].url = line.substring(sep + 1);
        }
        if (radioList[radioCount].url.length() > 0)
            radioCount++;
    }
    f.close();
}

void saveRadioList()
{
    SD.mkdir("/Music/_radio");
    File f = SD.open("/Music/_radio/webradio.cfg", FILE_WRITE);
    if (!f)
        return;
    for (int i = 0; i < radioCount; i++)
        f.printf("%s|%s\n", radioList[i].name.c_str(), radioList[i].url.c_str());
    f.close();
}

bool loadWifiConfig(String &ssid, String &pass)
{
    ssid = "";
    pass = "";
    File f = SD.open("/Music/_radio/wifi.cfg", FILE_READ);
    if (!f)
        return false;
    while (f.available())
    {
        String line = f.readStringUntil('\n');
        line.trim();
        int eq = line.indexOf('=');
        if (eq < 0)
            continue;
        String key = line.substring(0, eq);
        String val = line.substring(eq + 1);
        if (key == "ssid")
            ssid = val;
        if (key == "password")
            pass = val;
    }
    f.close();
    return ssid.length() > 0;
}

void saveWifiConfig(const String &ssid, const String &pass)
{
    SD.mkdir("/Music/_radio");
    File f = SD.open("/Music/_radio/wifi.cfg", FILE_WRITE);
    if (!f)
        return;
    f.printf("ssid=%s\npassword=%s\n", ssid.c_str(), pass.c_str());
    f.close();
}

String generateRadioName(const String &url, int n)
{
    int start = url.indexOf("://");
    if (start >= 0)
    {
        start += 3;
        int end = url.indexOf('/', start);
        String domain = (end > 0) ? url.substring(start, end) : url.substring(start);
        if (domain.startsWith("www."))
            domain = domain.substring(4);
        int dot = domain.lastIndexOf('.');
        if (dot > 0)
            domain = domain.substring(0, dot);
        domain.replace('-', ' ');
        if (domain.length() > 0 && domain.length() <= 20)
        {
            domain.toUpperCase();
            return domain;
        }
    }
    return "Radio " + String(n);
}

void purgeRadioMemory()
{
    stopRadioStream();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiConnected = false;
    delay(100);
}

void applyWifiPowerSave()
{
    if (!wifiConnected)
        return;
    if (radioIsPlaying)
        WiFi.setSleep(WIFI_PS_NONE);
    else
        WiFi.setSleep(wifiPowerSave ? WIFI_PS_MIN_MODEM : WIFI_PS_NONE);
}

// Pulls data into radioBuf's ring buffer until it reaches targetBytes or
// maxWaitMs elapses, whichever comes first. Called right after opening the
// stream and before handing it to the codec, so playback starts with a
// cushion instead of racing the network from byte zero.
void primeRadioBuffer(uint32_t targetBytes, unsigned long maxWaitMs)
{
    if (!radioBuf)
        return;
    showHdrMsg("BUFFERING...");
    unsigned long start = millis();
    while (millis() - start < maxWaitMs)
    {
        radioBuf->loop();
        if (radioBuf->getFillLevel() >= targetBytes)
            break;
        delay(5);
    }
    RDBG("[RADIO] prebuffered %luK in %lums\n",
         (unsigned long)(radioBuf->getFillLevel() / 1024), millis() - start);
}

void startRadioStream(int idx)
{
    if (!wifiConnected || idx < 0 || idx >= radioCount)
        return;
    int oldPlaying = radioPlaying;
    stopRadioStream();

    String url = radioList[idx].url;
    RDBG("[RADIO] start idx=%d url=%s free=%u\n", idx, url.c_str(), ESP.getFreeHeap());
    String loUrl = url;
    loUrl.toLowerCase();

    if (loUrl.endsWith(".ogg") || loUrl.indexOf("ogg") > 0)
    {
        showHdrMsg("OGG UNSUPPORTED");
        drawRadioAll();
        return;
    }

    // Use the FIXED class for BOTH HTTP and HTTPS to prevent aggressive reconnects
    // and ICY metadata issues (SomaFM uses chunked encoding with micro-pauses)
    httpSrc = new AudioFileSourceHTTPSStream(url.c_str());
    if (!httpSrc)
    {
        showHdrMsg("STREAM ALLOC FAIL");
        drawRadioAll();
        return;
    }
    static_cast<AudioFileSourceHTTPSStream *>(httpSrc)->SetReconnect(STREAM_RECONNECT_TRIES, STREAM_RECONNECT_DELAY_MS);
    radioBuf = new AudioFileSourceBuffer(httpSrc, RADIO_HTTP_BUF);
    if (!radioBuf)
    {
        showHdrMsg("BUFFER ALLOC FAIL");
        delete httpSrc;
        httpSrc = nullptr;
        drawRadioAll();
        return;
    }

    bool started = false;

    primeRadioBuffer(RADIO_PREBUFFER_TARGET, RADIO_PREBUFFER_MAX_WAIT_MS);

    if (loUrl.endsWith(".aac") || loUrl.indexOf("aac") > 0 || loUrl.indexOf("m4a") > 0 || radioForceAac)
    {
        aac = new AudioGeneratorAAC();
        started = aac->begin(radioBuf, output);
        if (!started)
        {
            delete aac;
            aac = nullptr;
        }
    }
    else
    {
        radioMp3 = new AudioGeneratorMP3();
        started = radioMp3->begin(radioBuf, output);

        if (!started)
        {
            delete radioMp3;
            radioMp3 = nullptr;

            // CRITICAL: The MP3 parser consumed bytes from radioBuf.
            // We must recreate the stream so AAC gets a fresh ADTS header.
            delete radioBuf;
            radioBuf = nullptr;
            httpSrc->close();
            delete httpSrc;
            httpSrc = nullptr;

            // Use the fixed class for both HTTP and HTTPS
            httpSrc = new AudioFileSourceHTTPSStream(url.c_str());
            static_cast<AudioFileSourceHTTPSStream *>(httpSrc)->SetReconnect(STREAM_RECONNECT_TRIES, STREAM_RECONNECT_DELAY_MS);
            radioBuf = new AudioFileSourceBuffer(httpSrc, RADIO_HTTP_BUF);
            primeRadioBuffer(RADIO_PREBUFFER_TARGET, RADIO_PREBUFFER_MAX_WAIT_MS);

            aac = new AudioGeneratorAAC();
            started = aac->begin(radioBuf, output);
            if (!started)
            {
                delete aac;
                aac = nullptr;
            }
        }
    }

    if (!started)
    {
        if (radioBuf)
        {
            delete radioBuf;
            radioBuf = nullptr;
        }
        if (httpSrc)
        {
            httpSrc->close();
            delete httpSrc;
            httpSrc = nullptr;
        }
        showHdrMsg("STREAM ERROR");
        RDBG("[RADIO] STREAM ERROR (begin failed)\n");
        drawRadioAll();
        return;
    }

    radioPlaying = idx;
    radioIsPlaying = true;
    applyWifiPowerSave();
    RDBG("[RADIO] playing codec=%s free=%u\n", aac ? "AAC" : "MP3", ESP.getFreeHeap());
    if (oldPlaying >= 0 && oldPlaying != idx)
        drawRadioRow(oldPlaying);
    drawRadioRow(idx);
    drawRadioHeader();
    drawRadioStatus();
}

// Cancels any pending auto-reconnect and resets the backoff counter. Call this
// on any deliberate user action (manual play/stop/select/exit) so a fresh
// action always starts a clean backoff sequence.
void cancelRadioReconnect()
{
    radioReconnectPending = false;
    radioReconnectAttempt = 0;
}

// Called when the stream drops mid-playback. Schedules another attempt with
// exponential backoff, up to RADIO_AUTO_RECONNECT_MAX tries, then gives up.
void scheduleRadioReconnect(int idx)
{
    if (idx < 0 || !wifiConnected)
    {
        radioReconnectPending = false;
        showHdrMsg("STREAM LOST");
        return;
    }

    radioReconnectAttempt++;
    if (radioReconnectAttempt > RADIO_AUTO_RECONNECT_MAX)
    {
        radioReconnectPending = false;
        showHdrMsg("STREAM LOST");
        RDBG("[RADIO] auto-reconnect gave up after %d tries\n", RADIO_AUTO_RECONNECT_MAX);
        return;
    }

    radioReconnectIdx = idx;
    radioReconnectPending = true;
    unsigned long backoffMs = 1000UL << min(radioReconnectAttempt - 1, 4); // 1,2,4,8,16s cap
    radioReconnectAtMs = millis() + backoffMs;

    char buf[24];
    snprintf(buf, sizeof(buf), "RECONNECT %d/%d", radioReconnectAttempt, RADIO_AUTO_RECONNECT_MAX);
    showHdrMsg(buf);
    RDBG("[RADIO] scheduling reconnect attempt %d in %lums\n", radioReconnectAttempt, backoffMs);
}

void stopRadioStream()
{
    if (radioMp3)
    {
        if (radioMp3->isRunning())
            radioMp3->stop();
        delete radioMp3;
        radioMp3 = nullptr;
    }
    if (aac)
    {
        if (aac->isRunning())
            aac->stop();
        delete aac;
        aac = nullptr;
    }
    if (radioBuf)
    {
        delete radioBuf;
        radioBuf = nullptr;
    }
    if (httpSrc)
    {
        httpSrc->close();
        delete httpSrc;
        httpSrc = nullptr;
    }
    if (output)
        output->stop();
    radioIsPlaying = false;
    radioPlaying = -1;
    applyWifiPowerSave();
    RDBG("[RADIO] stop\n");
}

void pumpRadioAudio()
{
    if (!radioIsPlaying)
    {
        // Not currently playing: if an auto-reconnect is scheduled and its
        // backoff delay has elapsed, retry now.
        if (webRadioMode && radioReconnectPending && millis() >= radioReconnectAtMs)
        {
            int idx = radioReconnectIdx;
            radioReconnectPending = false;
            startRadioStream(idx);
            if (radioIsPlaying)
            {
                cancelRadioReconnect();
                showHdrMsg("RECONNECTED");
            }
            else
            {
                scheduleRadioReconnect(idx);
            }
        }
        return;
    }

    if (radioBuf)
        radioBuf->loop();

    AudioGenerator *gen = radioMp3 ? (AudioGenerator *)radioMp3 : (aac ? (AudioGenerator *)aac : nullptr);
    if (!gen)
        return;

    if (!gen->isRunning())
    {
        int oldPlaying = radioPlaying;
        stopRadioStream();
        RDBG("[RADIO] STREAM LOST (gen stopped)\n");
        scheduleRadioReconnect(oldPlaying);
        if (oldPlaying >= 0)
            drawRadioRow(oldPlaying);
        drawRadioHeader();
        drawRadioStatus();
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        if (!gen->loop())
            break;
    }
}

void radioScrollEnsureVisible()
{
    if (radioSelected < radioScrollTop)
        radioScrollTop = radioSelected;
    if (radioSelected >= radioScrollTop + VISIBLE_TRACKS)
        radioScrollTop = radioSelected - VISIBLE_TRACKS + 1;
    if (radioScrollTop < 0)
        radioScrollTop = 0;
}

void scanWifiNetworks()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int n = WiFi.scanNetworks(false, false, false, 300);
    wifiNetCount = 0;
    for (int i = 0; i < n && wifiNetCount < WIFI_SCAN_MAX; i++)
    {
        wifiNets[wifiNetCount].ssid = WiFi.SSID(i);
        wifiNets[wifiNetCount].rssi = WiFi.RSSI(i);
        wifiNets[wifiNetCount].encrypted = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        if (wifiNets[wifiNetCount].ssid.length() > 0)
            wifiNetCount++;
    }
    WiFi.scanDelete();
}

bool connectWifi(const String &ssid, const String &pass)
{
    String truncSsid = ssid;
    if ((int)truncSsid.length() > 26)
        truncSsid = truncSsid.substring(0, 25) + ">";

    auto drawConnScreen = [&](uint8_t dotCount)
    {
        auto &D = M5Cardputer.Display;
        if (dotCount == 0)
        {
            D.fillRect(0, 0, SCREEN_W, SCREEN_H, T->hdrBg);
            D.drawRect(4, 4, SCREEN_W - 8, SCREEN_H - 8, T->accent1);
            D.setTextDatum(middle_center);
            D.setTextColor(T->accent2);
            D.drawString("WEB RADIO", SCREEN_W / 2, 18, 2);
            D.setTextColor(T->textMid);
            D.drawString(truncSsid, SCREEN_W / 2, SCREEN_H / 2, 1);
            D.setTextColor(T->textDim);
            D.drawString("DEL to cancel", SCREEN_W / 2, SCREEN_H / 2 + 16, 1);
        }
        D.fillRect(5, SCREEN_H / 2 - 24, SCREEN_W - 10, 16, T->hdrBg);
        char msg[18];
        snprintf(msg, sizeof(msg), "CONNECTING%.*s", dotCount, "...");
        D.setTextDatum(middle_center);
        D.setTextColor(T->accent1);
        D.drawString(msg, SCREEN_W / 2, SCREEN_H / 2 - 16, 1);
    };

    drawConnScreen(0);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long t = millis();
    unsigned long lastAnim = millis();
    uint8_t dots = 0;

    while (WiFi.status() != WL_CONNECTED && millis() - t < WIFI_TIMEOUT)
    {
        M5Cardputer.update();
        if (millis() - lastAnim >= 500)
        {
            dots = (dots % 3) + 1;
            drawConnScreen(dots);
            lastAnim = millis();
        }
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed())
        {
            Keyboard_Class::KeysState ks = M5Cardputer.Keyboard.keysState();
            if (ks.del)
            {
                WiFi.disconnect();
                return false;
            }
        }
        delay(50);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;
        wifiSSID = ssid;
        applyWifiPowerSave();
        RDBG("[WIFI] connected '%s' rssi=%d ip=%s free=%u\n",
             ssid.c_str(), WiFi.RSSI(), WiFi.localIP().toString().c_str(), ESP.getFreeHeap());
        saveWifiConfig(ssid, pass);
        return true;
    }
    WiFi.disconnect();
    wifiConnected = false;
    auto &D = M5Cardputer.Display;
    D.fillRect(0, 0, SCREEN_W, SCREEN_H, T->hdrBg);
    D.drawRect(4, 4, SCREEN_W - 8, SCREEN_H - 8, T->accent1);
    D.setTextDatum(middle_center);
    D.setTextColor(T->accent1);
    D.drawString("CONNECT FAILED", SCREEN_W / 2, SCREEN_H / 2 - 8, 1);
    D.setTextColor(T->textDim);
    D.drawString("any key to retry", SCREEN_W / 2, SCREEN_H / 2 + 8, 1);
    delay(2000);
    return false;
}

void showWifiOverlay()
{
    M5Cardputer.Display.fillRect(0, 0, SCREEN_W, SCREEN_H, T->hdrBg);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setTextColor(T->accent1);
    M5Cardputer.Display.drawString("SCANNING WIFI...", SCREEN_W / 2, SCREEN_H / 2, 1);
    scanWifiNetworks();
    wifiNetSel = 0;
    wifiNetScroll = 0;
    wifiOverlayVisible = true;
    drawWifiOverlay();
}

void showWifiPassOverlay(const String &ssid)
{
    inputSaved = ssid;
    inputBuf[0] = '\0';
    inputLen = 0;
    wifiPassOverlayVisible = true;
    drawWifiPassOverlay();
}

void showAddUrlOverlay()
{
    inputBuf[0] = '\0';
    inputLen = 0;
    addUrlStatusMsg[0] = '\0';
    addUrlAwaitingConfirm = false;
    addUrlOverlayVisible = true;
    drawAddUrlOverlay();
}

// Opens the URL just far enough to confirm the server responds, then closes
// it right away - reuses the exact same connect logic (headers, timeout,
// TLS handling) as real playback, so a pass here is a real signal, not just
// a format guess. Blocks for up to ~5s (the same timeout playback itself
// uses), which is fine for a one-off "adding a station" action.
bool testRadioUrl(const String &url)
{
    AudioFileSourceHTTPSStream testSrc;
    bool ok = testSrc.open(url.c_str());
    testSrc.close();
    return ok;
}

void showAddNameOverlay(const String &defaultName)
{
    strncpy(inputBuf, defaultName.c_str(), RADIO_INPUT_MAX);
    inputBuf[RADIO_INPUT_MAX] = '\0';
    inputLen = strlen(inputBuf);
    addNameOverlayVisible = true;
    drawAddNameOverlay();
}

void showRemoveConfirm()
{
    removeConfirmVisible = true;
    drawRemoveConfirm();
}

void handleOverlayInput(Keyboard_Class::KeysState &ks)
{
    if (wifiOverlayVisible)
    {
        for (auto c : ks.word)
        {
            if (c == ';' && wifiNetCount > 0)
            {
                wifiNetSel = (wifiNetSel - 1 + wifiNetCount) % wifiNetCount;
                if (wifiNetSel < wifiNetScroll)
                    wifiNetScroll = wifiNetSel;
                drawWifiOverlay();
            }
            if (c == '.' && wifiNetCount > 0)
            {
                wifiNetSel = (wifiNetSel + 1) % wifiNetCount;
                if (wifiNetSel >= wifiNetScroll + 6)
                    wifiNetScroll = wifiNetSel - 5;
                drawWifiOverlay();
            }
            if (c == 's' || c == 'S')
            {
                M5Cardputer.Display.setTextDatum(middle_center);
                M5Cardputer.Display.setTextColor(T->accent2);
                M5Cardputer.Display.drawString("SCANNING...", SCREEN_W / 2, 68, 1);
                scanWifiNetworks();
                wifiNetSel = 0;
                wifiNetScroll = 0;
                drawWifiOverlay();
            }
        }
        if (ks.enter && wifiNetCount > 0)
        {
            String ssid = wifiNets[wifiNetSel].ssid;
            String cfgSsid, cfgPass;
            wifiOverlayVisible = false;
            if (loadWifiConfig(cfgSsid, cfgPass) && cfgSsid == ssid)
            {
                if (connectWifi(ssid, cfgPass))
                    drawRadioAll();
                else
                    showWifiOverlay();
            }
            else
            {
                showWifiPassOverlay(ssid);
            }
        }
        if (ks.del)
        {
            wifiOverlayVisible = false;
            if (!wifiConnected)
                exitWebRadioMode();
            else
                drawRadioAll();
        }
    }
    else if (wifiPassOverlayVisible)
    {
        for (auto c : ks.word)
        {
            if (c >= 32 && c < 127 && inputLen < RADIO_INPUT_MAX)
            {
                inputBuf[inputLen++] = c;
                inputBuf[inputLen] = '\0';
                drawWifiPassOverlay(true);
            }
        }
        if (ks.del)
        {
            if (inputLen > 0)
            {
                inputBuf[--inputLen] = '\0';
                drawWifiPassOverlay(true);
            }
            else
            {
                wifiPassOverlayVisible = false;
                showWifiOverlay();
            }
        }
        if (ks.enter)
        {
            String pass = String(inputBuf);
            wifiPassOverlayVisible = false;
            if (connectWifi(inputSaved, pass))
                drawRadioAll();
            else
                showWifiOverlay();
        }
    }
    else if (addUrlOverlayVisible)
    {
        bool textChanged = false;
        for (auto c : ks.word)
        {
            if (c >= 32 && c < 127 && inputLen < RADIO_INPUT_MAX)
            {
                inputBuf[inputLen++] = c;
                inputBuf[inputLen] = '\0';
                textChanged = true;
            }
        }
        if (textChanged)
        {
            // Editing the URL invalidates any pending "add anyway?" confirm
            addUrlAwaitingConfirm = false;
            addUrlStatusMsg[0] = '\0';
            drawAddUrlOverlay(true);
        }
        if (ks.del)
        {
            if (inputLen > 0)
            {
                inputBuf[--inputLen] = '\0';
                addUrlAwaitingConfirm = false;
                addUrlStatusMsg[0] = '\0';
                drawAddUrlOverlay(true);
            }
            else
            {
                addUrlOverlayVisible = false;
                drawRadioAll();
            }
        }
        if (ks.enter && inputLen > 0)
        {
            inputSaved = String(inputBuf);

            if (addUrlAwaitingConfirm)
            {
                // Connectivity test failed last time, but user pressed ENTER
                // again to add it anyway (e.g. a stream that dislikes quick
                // connect/disconnect probes, or a temporarily-down server).
                addUrlAwaitingConfirm = false;
                addUrlOverlayVisible = false;
                addUrlStatusMsg[0] = '\0';
                String defName = generateRadioName(inputSaved, radioCount + 1);
                showAddNameOverlay(defName);
            }
            else if (!(inputSaved.startsWith("http://") || inputSaved.startsWith("https://")) || inputSaved.length() < 12)
            {
                strncpy(addUrlStatusMsg, "INVALID URL FORMAT", sizeof(addUrlStatusMsg) - 1);
                addUrlStatusMsg[sizeof(addUrlStatusMsg) - 1] = '\0';
                drawAddUrlOverlay(true);
            }
            else
            {
                strncpy(addUrlStatusMsg, "TESTING...", sizeof(addUrlStatusMsg) - 1);
                addUrlStatusMsg[sizeof(addUrlStatusMsg) - 1] = '\0';
                drawAddUrlOverlay(true);

                bool reachable = testRadioUrl(inputSaved);

                if (reachable)
                {
                    addUrlOverlayVisible = false;
                    addUrlStatusMsg[0] = '\0';
                    String defName = generateRadioName(inputSaved, radioCount + 1);
                    showAddNameOverlay(defName);
                }
                else
                {
                    strncpy(addUrlStatusMsg, "NO RESPONSE. ENTER=ADD ANYWAY", sizeof(addUrlStatusMsg) - 1);
                    addUrlStatusMsg[sizeof(addUrlStatusMsg) - 1] = '\0';
                    addUrlAwaitingConfirm = true;
                    drawAddUrlOverlay(true);
                }
            }
        }
    }
    else if (addNameOverlayVisible)
    {
        for (auto c : ks.word)
        {
            if (c >= 32 && c < 127 && inputLen < 31)
            {
                inputBuf[inputLen++] = c;
                inputBuf[inputLen] = '\0';
                drawAddNameOverlay(true);
            }
        }
        if (ks.del && inputLen > 0)
        {
            inputBuf[--inputLen] = '\0';
            drawAddNameOverlay(true);
        }
        if (ks.enter)
        {
            if (radioCount < RADIO_MAX)
            {
                radioList[radioCount].url = inputSaved;
                radioList[radioCount].name = (inputLen > 0) ? String(inputBuf) : ("Radio " + String(radioCount + 1));
                radioCount++;
                saveRadioList();
                radioSelected = radioCount - 1;
                radioScrollTop = max(0, radioSelected - VISIBLE_TRACKS + 1);
            }
            addNameOverlayVisible = false;
            drawRadioAll();
        }
    }
    else if (removeConfirmVisible)
    {
        if (ks.enter)
        {
            if (radioSelected == radioPlaying && radioIsPlaying)
                stopRadioStream();
            for (int i = radioSelected; i < radioCount - 1; i++)
                radioList[i] = radioList[i + 1];
            radioCount--;
            if (radioPlaying >= radioCount)
                radioPlaying = -1;
            if (radioSelected >= radioCount)
                radioSelected = max(0, radioCount - 1);
            radioScrollEnsureVisible();
            saveRadioList();
            removeConfirmVisible = false;
            drawRadioAll();
        }
        if (ks.del)
        {
            removeConfirmVisible = false;
            drawRadioAll();
        }
    }
}

void handleRadioInput(Keyboard_Class::KeysState &ks)
{
    // Any manual keypress in radio mode cancels a pending auto-reconnect and
    // resets its backoff, so the next drop starts a fresh retry sequence.
    cancelRadioReconnect();

    if (ks.del)
    {
        exitWebRadioMode();
        return;
    }

    if (ks.enter)
    {
        if (radioCount == 0)
        {
            showAddUrlOverlay();
            return;
        }
        if (radioIsPlaying && radioPlaying == radioSelected)
        {
            int oldPlaying = radioPlaying;
            stopRadioStream();
            drawRadioRow(oldPlaying);
            drawRadioHeader();
            drawRadioStatus();
        }
        else
        {
            startRadioStream(radioSelected);
        }
    }

    for (auto c : ks.word)
    {
        switch (c)
        {
        case ';':
            if (radioCount > 0)
            {
                int oldSel = radioSelected, oldScroll = radioScrollTop;
                radioSelected = (radioSelected - 1 + radioCount) % radioCount;
                radioScrollEnsureVisible();
                if (radioScrollTop != oldScroll)
                    drawRadioList();
                else
                {
                    drawRadioRow(oldSel);
                    drawRadioRow(radioSelected);
                }
            }
            break;
        case '.':
            if (radioCount > 0)
            {
                int oldSel = radioSelected, oldScroll = radioScrollTop;
                radioSelected = (radioSelected + 1) % radioCount;
                radioScrollEnsureVisible();
                if (radioScrollTop != oldScroll)
                    drawRadioList();
                else
                {
                    drawRadioRow(oldSel);
                    drawRadioRow(radioSelected);
                }
            }
            break;
        case ' ':
            if (radioIsPlaying && radioPlaying == radioSelected)
            {
                int oldPlaying = radioPlaying;
                stopRadioStream();
                drawRadioRow(oldPlaying);
                drawRadioHeader();
                drawRadioStatus();
            }
            else if (radioCount > 0)
            {
                startRadioStream(radioSelected);
            }
            break;
        case 'a':
        case 'A':
            if (radioCount < RADIO_MAX)
                showAddUrlOverlay();
            else
                showHdrMsg("LIST FULL");
            break;
        case 'x':
        case 'X':
            if (radioCount > 0)
                showRemoveConfirm();
            break;
        case 'r':
        case 'R':
            // Manual reconnect - helps with flaky modern streams that drop.
            if (!wifiConnected)
                showHdrMsg("NO WIFI");
            else if (radioIsPlaying && radioPlaying >= 0)
                startRadioStream(radioPlaying);
            else
                showHdrMsg("NOT PLAYING");
            break;
        case 'w':
        case 'W':
            exitWebRadioMode();
            return;
        case '+':
        case '=':
            volume = (uint8_t)min(255, (int)volume + 10);
            M5Cardputer.Speaker.setVolume(volume);
            settingsDirty = true;
            settingsDirtyMs = millis();
            drawRadioStatus();
            break;
        case '-':
            volume = (uint8_t)max(0, (int)volume - 10);
            M5Cardputer.Speaker.setVolume(volume);
            settingsDirty = true;
            settingsDirtyMs = millis();
            drawRadioStatus();
            break;
        case '1':
            setTheme(0);
            break;
        case '2':
            setTheme(1);
            break;
        case '3':
            setTheme(2);
            break;
        case '4':
            setTheme(3);
            break;
        case '5':
            setTheme(4);
            break;
        case 'o':
        case 'O':
            toggleScreen();
            break;
        case 'h':
        case 'H':
            toggleHelp();
            break;
        case 'm':
        case 'M':
            enterSettingsMenu();
            return;
        case 'd':
        case 'D':
            toggleDebug();
            return;
        case 'e':
        case 'E':
            enterEQScreen();
            return;
        case 'i':
        case 'I':
            radioForceAac = !radioForceAac;
            showHdrMsg(radioForceAac ? "FORCE AAC" : "AAC OFF");
            drawRadioStatus();
            break;
        }
    }
}

void enterWebRadioMode()
{
    cancelRadioReconnect();
    purgeAudioPlayerMemory();
    webRadioMode = true;
    radioSelected = 0;
    radioScrollTop = 0;
    loadRadioList();

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;
        wifiSSID = WiFi.SSID();
        drawRadioAll();
        return;
    }

    String cfgSsid, cfgPass;
    if (loadWifiConfig(cfgSsid, cfgPass) && cfgSsid.length() > 0)
    {
        if (connectWifi(cfgSsid, cfgPass))
        {
            drawRadioAll();
            return;
        }
    }
    showWifiOverlay();
}

void exitWebRadioMode()
{
    cancelRadioReconnect();
    purgeRadioMemory();
    webRadioMode = false;
    wifiOverlayVisible = false;
    wifiPassOverlayVisible = false;
    addUrlOverlayVisible = false;
    addNameOverlayVisible = false;
    removeConfirmVisible = false;
    helpVisible = false;

    allFolders.clear();
    allFolders.shrink_to_fit();
    scanLRUCount = 0;
    folderStack.clear();
    folderPage = 0;
    currentFolderIdx = 0;
    isRecentView = false;
    scanDir("/Music", "Music");
    loadFolderIdx(0);
    drawAll();
}