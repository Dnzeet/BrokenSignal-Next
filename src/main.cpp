#include <M5Cardputer.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include "State.h"
#include "UI.h"
#include "Player.h"
#include "Browser.h"
#include "Radio.h"

void setup()
{
  Serial.begin(115200);

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Speaker.begin();
  M5Cardputer.Speaker.setVolume(volume);
  M5Cardputer.Display.setRotation(1);

  drawSplash("> SCANNING /MUSIC/...");

  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  SPI.setFrequency(25000000); // 25mhz
  if (!SD.begin(SD_CS))
  {
    drawSplash("> SD CARD ERROR - REBOOT");
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setTextColor(rgb(255, 45, 120));
    M5Cardputer.Display.drawString("INSERT CARD", SCREEN_W / 2, SCREEN_H / 2 + 32, 1);
    while (true)
      delay(1000);
  }

  output = new AudioOutputM5Speaker(&M5Cardputer.Speaker, 0);
  statusCanvas.createSprite(SCREEN_W, STATUS_H);
  headerCanvas.createSprite(SCREEN_W, HEADER_H);

  allFolders.clear();
  scanDir("/Music", "Music");
  loadRecentFromSD();
  loadSettings();
  M5Cardputer.Display.setBrightness(screenBrightness);
  lastActivityMs = millis();
  loadFolderIdx(0);

  int totalTracks = (int)allFolders[0].tracks.size() + (int)allFolders[0].subFolderIds.size();
  if (totalTracks == 0)
  {
    drawSplash("> NO FILES IN /MUSIC/");
    while (true)
      delay(1000);
  }

  delay(300);
  drawAll();
}

void loop()
{
  M5Cardputer.update();

  if (webRadioMode)
    pumpRadioAudio();
  else
    pumpAudio();

  bool anyPlaying = isPlaying || radioIsPlaying;
  delay(anyPlaying ? 1 : 10);

  if (!isScanning && M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed())
  {
    Keyboard_Class::KeysState ks = M5Cardputer.Keyboard.keysState();

    lastActivityMs = millis();

    if (!screenOn)
    {
      wakeScreen();
      return;
    }

    if (settingsMenuVisible)
    {
      handleSettingsInput(ks);
      return;
    }

    if (eqScreenVisible)
    {
      handleEQInput(ks);
      return;
    }

    if (debugOverlayVisible)
    {
      for (auto c : ks.word)
        if (c == 'd' || c == 'D')
          toggleDebug();
      return;
    }

    if (helpVisible)
    {
      for (auto c : ks.word)
        if (c == 'h' || c == 'H')
          toggleHelp();
      return;
    }

    if (webRadioMode)
    {
      bool overlayOpen = wifiOverlayVisible || wifiPassOverlayVisible ||
                         addUrlOverlayVisible || addNameOverlayVisible ||
                         removeConfirmVisible;
      if (overlayOpen)
        handleOverlayInput(ks);
      else
        handleRadioInput(ks);
      return;
    }

    if (ks.enter)
      enterItem(selectedItem);
    if (ks.del)
      goBack();

    for (auto c : ks.word)
    {
      switch (c)
      {
      case ';':
        if (!items.empty())
        {
          selectedItem = (selectedItem - 1 + (int)items.size()) % (int)items.size();
          drawTrackList();
        }
        break;
      case '.':
        if (!items.empty())
        {
          selectedItem = (selectedItem + 1) % (int)items.size();
          drawTrackList();
        }
        break;
      case ',':
        if (isPlaying || isPaused)
        {
          seekTrack(-(int)seekSeconds * 1000);
        }
        else if (folderPage > 0)
        {
          stopAudio();
          folderPage--;
          loadFolderIdx(currentFolderIdx);
          selectedItem = 0;
          drawAll();
        }
        else
        {
          goBack();
        }
        break;
      case '/':
        if (isPlaying || isPaused)
        {
          seekTrack((int)seekSeconds * 1000);
        }
        else
        {
          int total = allFolders[currentFolderIdx].totalItems;
          if ((folderPage + 1) * PAGE_SIZE < total)
          {
            stopAudio();
            folderPage++;
            loadFolderIdx(currentFolderIdx);
            selectedItem = 0;
            drawAll();
          }
        }
        break;
      case ' ':
        if (isPlaying)
          pauseAudio();
        else if (isPaused)
          resumeAudio();
        else
        {
          int t = (currentTrack >= 0) ? currentTrack : -1;
          if (t < 0)
          {
            for (int i = 0; i < (int)items.size(); i++)
              if (!items[i].isFolder)
              {
                t = i;
                break;
              }
          }
          if (t >= 0)
            startTrack(t);
        }
        break;
      case '+':
      case '=':
        volume = (uint8_t)min(255, (int)volume + 10);
        M5Cardputer.Speaker.setVolume(volume);
        settingsDirty = true;
        settingsDirtyMs = millis();
        drawStatus();
        break;
      case '-':
        volume = (uint8_t)max(0, (int)volume - 10);
        M5Cardputer.Speaker.setVolume(volume);
        settingsDirty = true;
        settingsDirtyMs = millis();
        drawStatus();
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
      case 'r':
      case 'R':
        cycleRepeat();
        break;
      case 's':
      case 'S':
        toggleShuffle();
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
      case 'w':
      case 'W':
        enterWebRadioMode();
        return;
      }
    }
  }

  static unsigned long lastDraw = 0;
  if (screenOn && !helpVisible && !settingsMenuVisible && !eqScreenVisible && millis() - lastDraw >= 500)
  {
    lastDraw = millis();
    cursorVisible = !cursorVisible;

    if (debugOverlayVisible)
    {
      drawDebug();
    }
    else if (webRadioMode)
    {
      bool overlayOpen = wifiOverlayVisible || wifiPassOverlayVisible ||
                         addUrlOverlayVisible || addNameOverlayVisible ||
                         removeConfirmVisible;
      if (!overlayOpen)
      {
        if (radioIsPlaying)
        {
          drawRadioHeader();
          // Live buffer-fill % in the status bar - only worth the redraw
          // while actually streaming, otherwise it never changes.
          drawRadioStatus();
        }
        if (themeIdx == 1 && radioCount > 0)
        {
          if (!radioIsPlaying)
            drawRadioHeader();
          int visIdx = radioSelected - radioScrollTop;
          if (visIdx >= 0 && visIdx < VISIBLE_TRACKS)
          {
            int y = LIST_Y + visIdx * LIST_ITEM_H;
            uint16_t cur = cursorVisible ? T->accent1 : T->bg;
            M5Cardputer.Display.fillRect(SCREEN_W - 7 - 8, y + 3, 5, LIST_ITEM_H - 6, cur);
          }
        }
      }
    }
    else
    {
      bool terminalNeedsBlink = (themeIdx == 1) &&
                                (isPlaying || isPaused ||
                                 (selectedItem >= 0 && selectedItem < (int)items.size() &&
                                  !items[selectedItem].isFolder));
      bool headerNeedsBlink = terminalNeedsBlink || isPlaying;
      if (headerNeedsBlink)
        drawHeader();
      if (isPlaying)
        drawStatus();
    }
  }

  if (toastActive && millis() > toastEnd)
  {
    toastActive = false;
    if (screenOn && !helpVisible && !settingsMenuVisible && !eqScreenVisible)
      drawAll();
  }

  if (hdrMsgEnd > 0 && millis() >= hdrMsgEnd)
  {
    hdrMsgEnd = 0;
    if (screenOn && !helpVisible && !settingsMenuVisible && !eqScreenVisible)
    {
      if (webRadioMode)
        drawRadioHeader();
      else
        drawHeader();
    }
  }

  // Flush settings.cfg via two paths:
  //  - Fast path: 2s after the last change, but only when nothing is
  //    actively streaming (avoids SD writes competing with live audio I/O).
  //  - Safety net: if a change has sat unsaved for SETTINGS_FORCE_SAVE_INTERVAL_MS
  //    (default 5 min) even during a long playback session, flush anyway -
  //    a crash or power loss shouldn't wipe out settings from an hour of
  //    listening just because playback never paused.
  if (settingsDirty)
  {
    bool idle = !isPlaying && !isPaused && !radioIsPlaying;
    unsigned long dirtyForMs = millis() - settingsDirtyMs;
    if ((idle && dirtyForMs >= SETTINGS_IDLE_SAVE_DELAY_MS) ||
        dirtyForMs >= SETTINGS_FORCE_SAVE_INTERVAL_MS)
    {
      saveSettings();
      settingsDirty = false;
    }
  }

  if (batteryLevel < 0 || millis() - batteryLastMs >= BATTERY_INTERVAL)
  {
    batteryLevel = (int)min((int32_t)99, M5.Power.getBatteryLevel());
    batteryLastMs = millis();
    if (screenOn && !helpVisible && !settingsMenuVisible && !eqScreenVisible)
    {
      if (webRadioMode)
        drawRadioStatus();
      else
        drawStatus();
    }
  }

  // Battery-safe auto screen off: only when idle, never over an open menu/overlay.
  if (screenOn && autoScreenOffSec > 0 && !settingsMenuVisible && !helpVisible &&
      !eqScreenVisible && !debugOverlayVisible &&
      !wifiOverlayVisible && !wifiPassOverlayVisible && !addUrlOverlayVisible &&
      !addNameOverlayVisible && !removeConfirmVisible &&
      millis() - lastActivityMs >= (unsigned long)autoScreenOffSec * 1000UL)
  {
    screenOn = false;
    M5Cardputer.Display.setBrightness(0);
  }
}