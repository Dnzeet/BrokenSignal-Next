#include "UI.h"
#include "Utils.h"
#include "Player.h"
#include "Browser.h"
#include "Radio.h"

void drawSplash(const char *statusLine)
{
    uint16_t bg = rgb(2, 6, 4);
    uint16_t mGreen1 = rgb(0, 255, 65);
    uint16_t mGreen2 = rgb(0, 140, 30);
    uint16_t mGreen3 = rgb(0, 50, 10);
    uint16_t cyan = rgb(0, 245, 255);
    uint16_t magenta = rgb(255, 45, 120);
    uint16_t red = rgb(255, 30, 50);
    uint16_t yellow = rgb(245, 230, 66);

    M5Cardputer.Display.fillScreen(bg);

    for (int x = 4; x < SCREEN_W; x += 12)
    {
        int StreamStart = (x * 7) % (SCREEN_H / 2);
        int StreamLen = 30 + ((x * 13) % 60);

        for (int y = StreamStart; y < StreamStart + StreamLen && y < SCREEN_H; y += 6)
        {
            uint16_t dotColor = (y < StreamStart + 12) ? mGreen1 : ((y < StreamStart + 35) ? mGreen2 : mGreen3);

            M5Cardputer.Display.drawFastVLine(x, y, 3, dotColor);
        }
    }

    M5Cardputer.Display.fillRect(0, 22, 240, 2, mGreen2);
    M5Cardputer.Display.fillRect(0, 23, 120, 2, mGreen3);
    M5Cardputer.Display.fillRect(18, 44, 80, 1, magenta);
    M5Cardputer.Display.fillRect(140, 44, 60, 1, mGreen2);
    M5Cardputer.Display.fillRect(0, 88, 240, 2, mGreen2);
    M5Cardputer.Display.fillRect(60, 89, 180, 1, mGreen3);
    M5Cardputer.Display.fillRect(10, 105, 50, 1, red);
    M5Cardputer.Display.fillRect(180, 105, 40, 1, mGreen2);

    M5Cardputer.Display.drawRect(0, 0, SCREEN_W, SCREEN_H, mGreen2);
    M5Cardputer.Display.drawRect(2, 2, SCREEN_W - 4, SCREEN_H - 4, mGreen3);

    M5Cardputer.Display.drawFastHLine(0, 0, 10, cyan);
    M5Cardputer.Display.drawFastVLine(0, 0, 10, cyan);
    M5Cardputer.Display.drawFastHLine(SCREEN_W - 10, 0, 10, cyan);
    M5Cardputer.Display.drawFastVLine(SCREEN_W - 1, 0, 10, cyan);
    M5Cardputer.Display.drawFastHLine(0, SCREEN_H - 1, 10, cyan);
    M5Cardputer.Display.drawFastVLine(0, SCREEN_H - 10, 10, cyan);
    M5Cardputer.Display.drawFastHLine(SCREEN_W - 10, SCREEN_H - 1, 10, cyan);
    M5Cardputer.Display.drawFastVLine(SCREEN_W - 1, SCREEN_H - 10, 10, cyan);

    M5Cardputer.Display.setTextDatum(middle_left);
    M5Cardputer.Display.setTextColor(mGreen2);
    M5Cardputer.Display.drawString("// SYS:BOOT //", 8, 10, 1);

    M5Cardputer.Display.setTextDatum(middle_right);
    M5Cardputer.Display.setTextColor(red);
    M5Cardputer.Display.drawString("ERR_SIG", SCREEN_W - 8, 10, 1);

    int titleY1 = 46;
    int titleY2 = 68;
    int titleY3 = 90;
    M5Cardputer.Display.setTextDatum(middle_center);

    M5Cardputer.Display.setTextColor(mGreen3);
    M5Cardputer.Display.drawString("BROKEN", SCREEN_W / 2 - 2, titleY1, 4);
    M5Cardputer.Display.setTextColor(magenta);
    M5Cardputer.Display.drawString("BROKEN", SCREEN_W / 2 + 1, titleY1, 4);
    M5Cardputer.Display.setTextColor(rgb(230, 255, 235));
    M5Cardputer.Display.drawString("BROKEN", SCREEN_W / 2, titleY1, 4);

    M5Cardputer.Display.setTextColor(mGreen3);
    M5Cardputer.Display.drawString("SIGNAL", SCREEN_W / 2 - 1, titleY2, 4);
    M5Cardputer.Display.setTextColor(cyan);
    M5Cardputer.Display.drawString("SIGNAL", SCREEN_W / 2 + 1, titleY2, 4);
    M5Cardputer.Display.setTextColor(rgb(200, 255, 210));
    M5Cardputer.Display.drawString("SIGNAL", SCREEN_W / 2, titleY2, 4);

    M5Cardputer.Display.setTextColor(magenta);
    M5Cardputer.Display.drawString("NEXT", SCREEN_W / 2 - 1, titleY3, 4);
    M5Cardputer.Display.setTextColor(mGreen2);
    M5Cardputer.Display.drawString("NEXT", SCREEN_W / 2 + 1, titleY3, 4);
    M5Cardputer.Display.setTextColor(yellow);
    M5Cardputer.Display.drawString("NEXT", SCREEN_W / 2, titleY3, 4);

    M5Cardputer.Display.setTextColor(mGreen1);
    M5Cardputer.Display.drawString(statusLine, SCREEN_W / 2, SCREEN_H - 12, 1);
}
void drawAll()
{
    if (webRadioMode)
    {
        drawRadioAll();
        return;
    }
    drawHeader();
    pumpAudio();
    drawTrackList();
    pumpAudio();
    drawStatus();
}

void showHdrMsg(const char *msg)
{
    strncpy(hdrMsg, msg, sizeof(hdrMsg) - 1);
    hdrMsg[sizeof(hdrMsg) - 1] = '\0';
    hdrMsgEnd = millis() + 1000;
    if (webRadioMode)
        drawRadioHeader();
    else
        drawHeader();
}

void showToast()
{
    int tw = SCREEN_W - 40, th = 18, tx = 20, ty = (SCREEN_H - th) / 2;
    M5Cardputer.Display.fillRoundRect(tx, ty, tw, th, 3, T->accent1);
    M5Cardputer.Display.setTextColor(T->bg);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.drawString(T->name, SCREEN_W / 2, ty + th / 2, 1);
    toastActive = true;
    toastEnd = millis() + 750;
}

void cycleRepeat()
{
    repeatMode = (repeatMode + 1) % 3;
    settingsDirty = true;
    settingsDirtyMs = millis();
    const char *labels[] = {"REPEAT OFF", "REPEAT ONE", "REPEAT ALL"};
    showHdrMsg(labels[repeatMode]);
}

void toggleShuffle()
{
    shuffleOn = !shuffleOn;
    settingsDirty = true;
    settingsDirtyMs = millis();
    showHdrMsg(shuffleOn ? "SHUFFLE ON" : "SHUFFLE OFF");
}

void setTheme(uint8_t idx)
{
    if (idx >= 5)
        return;
    themeIdx = idx;
    T = THEMES[idx];
    settingsDirty = true;
    settingsDirtyMs = millis();
    drawAll();
    showToast();
}

void toggleScreen()
{
    screenOn = !screenOn;
    M5Cardputer.Display.setBrightness(screenOn ? screenBrightness : 0);
    if (screenOn)
    {
        lastActivityMs = millis();
        drawAll();
    }
}

void wakeScreen()
{
    screenOn = true;
    M5Cardputer.Display.setBrightness(screenBrightness);
    lastActivityMs = millis();
    drawAll();
}

void drawHeader()
{
#define D headerCanvas
    D.fillRect(0, 0, SCREEN_W, HEADER_H, T->hdrBg);

    String name;
    if (currentTrack >= 0 && currentTrack < (int)items.size())
        name = shortName(items[currentTrack].path, 18);
    else if (viewFolder == "__RECENT__")
        name = "RECENT";
    else if (viewFolder != "/Music")
        name = folderName(viewFolder, 24);
    else
        name = "---";

    int trackCount = 0, trackPos = 0;
    for (int i = 0; i < (int)items.size(); i++)
    {
        if (!items[i].isFolder)
        {
            trackCount++;
            if (i == currentTrack)
                trackPos = trackCount;
        }
    }
    char countStr[12];
    snprintf(countStr, sizeof(countStr), "%d/%d", trackPos, trackCount);
    uint16_t stateCol = isPlaying ? T->accent1 : (isPaused ? T->accent2 : T->textDim);

    if (themeIdx == 0)
    {
        D.fillRect(0, 0, 3, HEADER_H, T->accent1);
        const char *icon = isPlaying ? ">" : (isPaused ? "||" : "-");
        uint16_t iconCol = (isPlaying && !cursorVisible) ? T->hdrBg : stateCol;
        D.setTextDatum(middle_left);
        D.setTextColor(iconCol);
        D.drawString(icon, 8, 7, 1);

        D.setTextColor(T->textDim);
        String subline;
        if (!isPlaying && !isPaused && currentFolderIdx != 0)
            subline = "/ " + (isRecentView ? String("RECENT") : folderName(viewFolder, 18));
        else
            subline = isPlaying ? "// PLAYING NOW //" : (isPaused ? "// PAUSED //" : "// STOPPED //");
        D.drawString(subline, 20, 7, 1);

        D.setTextColor(T->accent3);
        D.setTextDatum(middle_right);
        D.drawString(countStr, SCREEN_W - 4, 7, 1);
        D.setTextColor(T->textDim);
        D.drawString("H", SCREEN_W - 4 - (int)strlen(countStr) * 6 - 8, 7, 1);

        D.setTextColor(T->accent2);
        D.setTextDatum(middle_left);
        D.drawString(name, 8, 22, 2);

        D.drawFastHLine(3, HEADER_H - 2, SCREEN_W - 3, T->accent1);
        D.drawFastHLine(3, HEADER_H - 1, SCREEN_W - 3, T->textDim);

        D.drawFastHLine(0, 0, 6, T->accent1);
        D.drawFastVLine(0, 0, 6, T->accent1);
        D.drawFastHLine(SCREEN_W - 6, 0, 6, T->accent1);
        D.drawFastVLine(SCREEN_W - 1, 0, 6, T->accent1);
    }
    else if (themeIdx == 1)
    {
        D.setTextDatum(middle_left);
        D.setTextColor(T->textDim);
        D.drawString("BRKN_SIGNAL //", 4, 7, 1);
        D.setTextColor(stateCol);
        String stateTag;
        if (!isPlaying && !isPaused && currentFolderIdx != 0)
            stateTag = isRecentView ? "RECENT" : folderName(viewFolder, 10);
        else
            stateTag = isPlaying ? "PLAYING" : (isPaused ? "PAUSED" : "STOPPED");
        D.drawString(stateTag, 110, 7, 1);

        D.setTextColor(T->accent2);
        D.setTextDatum(middle_right);
        char csBracketed[16];
        snprintf(csBracketed, sizeof(csBracketed), "[%s]", countStr);
        D.drawString(csBracketed, SCREEN_W - 4, 7, 1);
        D.setTextColor(T->textDim);
        int counterW = ((int)strlen(countStr) + 2) * 6 + 8;
        D.drawString("H", SCREEN_W - 4 - counterW, 7, 1);

        const int CUR_X = 4, CUR_W = 5, CUR_GAP = 3;
        D.fillRect(CUR_X, 16, CUR_W, 10, cursorVisible ? T->accent1 : T->hdrBg);

        D.setTextColor(T->accent1);
        D.setTextDatum(middle_left);
        D.drawString(name, CUR_X + CUR_W + CUR_GAP, 22, 2);

        for (int x = 0; x < SCREEN_W; x += 5)
            D.drawFastHLine(x, HEADER_H - 1, 3, T->textDim);
    }
    else
    {
        D.fillRect(0, 0, 3, HEADER_H, T->accent1);
        D.drawFastVLine(0, 0, HEADER_H / 2, T->accent2);

        for (int row = 0; row < HEADER_H; row++)
        {
            int lx = SCREEN_W - 36 + row / 4;
            if (lx < SCREEN_W)
                D.drawPixel(lx, row, T->textDim);
        }

        D.setTextDatum(middle_left);
        D.setTextColor(stateCol);
        if (isPlaying)
        {
            uint16_t dotCol = cursorVisible ? stateCol : T->hdrBg;
            D.setTextColor(dotCol);
            D.drawString("*", 8, 7, 1);
            D.setTextColor(stateCol);
            D.drawString("NOW PLAYING", 18, 7, 1);
        }
        else if (!isPaused && currentFolderIdx != 0)
        {
            D.setTextColor(T->textDim);
            D.drawString("/ " + (isRecentView ? String("RECENT") : folderName(viewFolder, 14)), 8, 7, 1);
        }
        else
        {
            D.drawString(isPaused ? "* PAUSED" : "* STOPPED", 8, 7, 1);
        }

        D.setTextColor(T->accent1);
        D.setTextDatum(middle_right);
        D.drawString(countStr, SCREEN_W - 4, 7, 1);
        D.setTextColor(T->textDim);
        D.drawString("H", SCREEN_W - 4 - (int)strlen(countStr) * 6 - 8, 7, 1);

        D.setTextColor(T->textBright);
        D.setTextDatum(middle_left);
        D.drawString(name, 8, 22, 2);

        D.drawFastHLine(3, HEADER_H - 1, SCREEN_W - 3, T->textDim);
    }

    if (hdrMsgEnd > 0 && millis() < hdrMsgEnd)
    {
        D.setTextDatum(middle_right);
        D.setTextColor(T->accent2);
        D.drawString(hdrMsg, SCREEN_W - 4, 22, 1);
    }

    if (themeIdx == 3)
    {
        D.drawFastHLine(0, HEADER_H - 2, SCREEN_W, T->accent2);
        D.drawFastHLine(0, HEADER_H - 1, SCREEN_W, rgb(0, 80, 70));
    }
    else if (themeIdx == 4)
    {
        D.drawFastHLine(0, HEADER_H - 1, SCREEN_W, T->accent1);
    }

    headerCanvas.pushSprite(0, 0);
#undef D
}

void drawTrackList()
{
    int listH = VISIBLE_TRACKS * LIST_ITEM_H;
    if (items.empty())
    {
        M5Cardputer.Display.fillRect(0, HEADER_H, SCREEN_W, STATUS_Y - HEADER_H, T->bg);
        return;
    }

    int top = selectedItem - VISIBLE_TRACKS / 2;
    top = max(0, min(top, (int)items.size() - VISIBLE_TRACKS));
    if (top < 0)
        top = 0;

    int sbW = (themeIdx == 1) ? 7 : 3;

    M5Cardputer.Display.fillRect(SCREEN_W - 7, LIST_Y, 7, VISIBLE_TRACKS * LIST_ITEM_H, T->bg);

    int visibleCount = min(VISIBLE_TRACKS, (int)items.size() - top);
    for (int i = visibleCount; i < VISIBLE_TRACKS; i++)
    {
        int y = LIST_Y + i * LIST_ITEM_H;
        M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, T->bg);
    }
    M5Cardputer.Display.drawFastHLine(0, HEADER_H, SCREEN_W, T->bg);
    M5Cardputer.Display.drawFastHLine(0, STATUS_Y - 1, SCREEN_W, T->bg);

    uint16_t folderCol = T->accent3;
    uint16_t folderSelCol = T->accent2;
    if (themeIdx == 1 || themeIdx == 2)
    {
        folderCol = T->accent2;
        folderSelCol = T->accent1;
    }

    int tNumPrefix[64] = {};
    for (int i = 0; i < (int)items.size() && i < 63; i++)
        tNumPrefix[i + 1] = tNumPrefix[i] + (items[i].isFolder ? 0 : 1);

    for (int i = 0; i < VISIBLE_TRACKS; i++)
    {
        int idx = top + i;
        if (idx >= (int)items.size())
            break;

        int y = LIST_Y + i * LIST_ITEM_H;
        bool sel = (idx == selectedItem);
        bool play = (!items[idx].isFolder && idx == currentTrack);
        bool isFolder = items[idx].isFolder;

        bool isPagination = (items[idx].path == "__PREV__" || items[idx].path == "__MORE__");
        if (isPagination)
        {
            M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, sel ? T->selRow : T->bg);
            uint16_t pc = sel ? T->accent1 : T->textDim;
            M5Cardputer.Display.setTextColor(pc);
            M5Cardputer.Display.setTextDatum(middle_center);
            M5Cardputer.Display.drawString(items[idx].label, SCREEN_W / 2, y + LIST_ITEM_H / 2, 1);
            M5Cardputer.Display.setTextDatum(middle_left);
            continue;
        }

        if (themeIdx == 0)
        {
            if (sel)
            {
                M5Cardputer.Display.fillRect(0, y, SCREEN_W / 2, LIST_ITEM_H, T->selRow);
                M5Cardputer.Display.fillRect(SCREEN_W / 2, y, SCREEN_W / 2, LIST_ITEM_H, T->bg);
            }
            else if (isFolder)
            {
                M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, rgb(14, 12, 4));
            }
            else
            {
                M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, T->bg);
            }
            M5Cardputer.Display.drawFastHLine(0, y + LIST_ITEM_H - 1, SCREEN_W, rgb(9, 21, 32));
        }
        else if (themeIdx == 1)
        {
            uint16_t rowBg = sel        ? T->selRow
                             : isFolder ? rgb(10, 8, 0)
                                        : T->bg;
            M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, rowBg);
        }
        else
        {
            if (sel)
            {
                M5Cardputer.Display.fillRect(0, y, 80, LIST_ITEM_H, rgb(20, 16, 10));
                M5Cardputer.Display.fillRect(80, y, SCREEN_W - 80, LIST_ITEM_H, T->bg);
            }
            else if (isFolder)
            {
                M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, rgb(14, 11, 4));
            }
            else
            {
                M5Cardputer.Display.fillRect(0, y, SCREEN_W, LIST_ITEM_H, T->bg);
            }
            M5Cardputer.Display.drawFastHLine(0, y + LIST_ITEM_H - 1, SCREEN_W, T->textDim);
        }

        if (themeIdx == 0)
        {
            if (play)
                M5Cardputer.Display.fillRect(3, y, 3, LIST_ITEM_H, T->accent1);
            else if (sel && isFolder)
                M5Cardputer.Display.fillRect(3, y, 2, LIST_ITEM_H, folderSelCol);
            else if (sel)
                M5Cardputer.Display.fillRect(3, y, 2, LIST_ITEM_H, T->accent2);
        }
        else if (themeIdx == 2)
        {
            if (play)
                M5Cardputer.Display.fillRect(3, y + 3, 3, LIST_ITEM_H - 6, T->accent1);
            else if (sel && isFolder)
                M5Cardputer.Display.fillRect(3, y + 3, 3, LIST_ITEM_H - 6, folderSelCol);
            if (sel)
            {
                M5Cardputer.Display.drawFastHLine(SCREEN_W - sbW - 7, y + 3, 5, T->accent2);
                M5Cardputer.Display.drawFastVLine(SCREEN_W - sbW - 3, y + 3, 5, T->accent2);
            }
        }
        else if (themeIdx == 3)
        {
            if (play)
                M5Cardputer.Display.fillRect(0, y, 3, LIST_ITEM_H, T->accent1);
            else if (sel)
                M5Cardputer.Display.fillRect(0, y, 3, LIST_ITEM_H, T->accent2);
        }
        else if (themeIdx == 4)
        {
            if (sel)
                M5Cardputer.Display.drawFastVLine(0, y, LIST_ITEM_H, T->accent1);
            if (play)
                M5Cardputer.Display.drawFastVLine(1, y, LIST_ITEM_H, T->accent2);
        }

        int midY = y + LIST_ITEM_H / 2;

        if (isFolder)
        {
            if (themeIdx == 1)
            {
                M5Cardputer.Display.setTextDatum(middle_left);
                M5Cardputer.Display.setTextColor(sel ? folderSelCol : folderCol);
                M5Cardputer.Display.drawString("#", 2, midY, 1);
            }
        }
        else
        {
            if (themeIdx == 1)
            {
                M5Cardputer.Display.setTextDatum(middle_left);
                if (play)
                {
                    M5Cardputer.Display.setTextColor(T->accent1);
                    M5Cardputer.Display.drawString(">", 2, midY, 1);
                }
                else if (sel)
                {
                    M5Cardputer.Display.setTextColor(T->accent2);
                    M5Cardputer.Display.drawString("|", 2, midY, 1);
                }
                M5Cardputer.Display.setTextColor(T->textDim);
                int tNum = tNumPrefix[idx + 1];
                char numBuf[4];
                snprintf(numBuf, sizeof(numBuf), "%02d", tNum);
                M5Cardputer.Display.drawString(numBuf, 12, midY, 1);
            }
            else
            {
                int tNum = tNumPrefix[idx + 1];
                char numBuf[4];
                snprintf(numBuf, sizeof(numBuf), "%02d", tNum);
                uint16_t nc = (themeIdx == 4) ? T->textDim
                              : play          ? T->accent1
                                              : (sel ? T->accent2 : T->textDim);
                M5Cardputer.Display.setTextColor(nc);
                M5Cardputer.Display.setTextDatum(middle_left);
                M5Cardputer.Display.drawString(numBuf, 8, midY, 1);
            }
        }

        M5Cardputer.Display.setTextDatum(middle_left);
        int nameX = (themeIdx == 1) ? 30 : 26;

        if (isFolder)
        {
            uint16_t fc = sel ? folderSelCol : folderCol;
            M5Cardputer.Display.setTextColor(fc);
            M5Cardputer.Display.drawString("/", nameX, midY, 1);
            String lbl = items[idx].label;
            int maxFolderChars = (themeIdx == 1) ? 32 : 33;
            if ((int)lbl.length() > maxFolderChars)
                lbl = lbl.substring(0, maxFolderChars - 1) + ">";
            M5Cardputer.Display.setTextColor(fc);
            M5Cardputer.Display.drawString(lbl, nameX + 10, midY, 1);
        }
        else
        {
            uint16_t nameCol;
            if (themeIdx == 1)
                nameCol = play ? T->accent1 : (sel ? T->accent2 : T->textMid);
            else
                nameCol = play ? T->accent1 : (sel ? T->textBright : T->textMid);
            M5Cardputer.Display.setTextColor(nameCol);
            String lbl = items[idx].label;
            int maxTrackChars = (themeIdx == 1) ? 33 : (themeIdx == 2) ? 30
                                                                       : 35;
            if ((int)lbl.length() > maxTrackChars)
                lbl = lbl.substring(0, maxTrackChars - 1) + ">";
            M5Cardputer.Display.drawString(lbl, nameX, midY, 1);
        }

        if (themeIdx == 0 && play)
        {
            uint16_t glowCol = rgb(80, 0, 40);
            M5Cardputer.Display.drawFastHLine(0, y, SCREEN_W, glowCol);
            M5Cardputer.Display.drawFastHLine(0, y + LIST_ITEM_H - 1, SCREEN_W, glowCol);
        }

        if (themeIdx == 1 && sel && !isFolder)
        {
            uint16_t cur = cursorVisible ? T->accent1 : T->bg;
            M5Cardputer.Display.fillRect(SCREEN_W - sbW - 8, y + 3, 5, LIST_ITEM_H - 6, cur);
        }

        if (themeIdx == 2 && !isFolder && items[idx].durationMs > 0)
        {
            char dur[8];
            {
                unsigned long s = items[idx].durationMs / 1000;
                snprintf(dur, sizeof(dur), "%02lu:%02lu", s / 60, s % 60);
            }
            M5Cardputer.Display.setTextDatum(middle_right);
            M5Cardputer.Display.setTextColor(play ? T->accent1 : T->textDim);
            M5Cardputer.Display.drawString(dur, SCREEN_W - sbW - 2, midY, 1);
            M5Cardputer.Display.setTextDatum(middle_left);
        }
    }

    if ((int)items.size() > VISIBLE_TRACKS)
    {
        int sbH = listH;
        int thH = max(5, sbH * VISIBLE_TRACKS / (int)items.size());
        int thY = LIST_Y + (sbH - thH) * top / max(1, (int)items.size() - VISIBLE_TRACKS);
        int sbX = SCREEN_W - sbW;
        if (themeIdx == 1)
        {
            M5Cardputer.Display.drawFastVLine(sbX, LIST_Y, sbH, T->textDim);
            M5Cardputer.Display.fillRect(sbX + 1, thY, sbW - 1, thH, T->textDim);
            M5Cardputer.Display.drawRect(sbX + 1, thY, sbW - 1, thH, T->textMid);
        }
        else
        {
            M5Cardputer.Display.fillRect(sbX, LIST_Y, sbW, sbH, T->barBg);
            M5Cardputer.Display.fillRect(sbX, thY, sbW, thH,
                                         themeIdx == 2 ? T->accent2 : T->accent1);
        }
    }
}

void drawStatus()
{
    unsigned long elapsed = pausedElapsedMs + (isPlaying ? millis() - trackStartMs : 0UL);
    if (elapsed > trackDurationMs && trackDurationMs > 0)
        elapsed = trackDurationMs;
    float prog = (trackDurationMs > 0)
                     ? min(1.0f, (float)elapsed / (float)trackDurationMs)
                     : 0.0f;
    int volPct = (volume * 100) / 255;

    statusCanvas.fillSprite(T->hdrBg);

    if (themeIdx == 0)
    {
        const int BAR_X = 72, BAR_W = 120, BAR_H = 6;
        const int BAR_Y = (STATUS_H - BAR_H) / 2;

        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);

        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString(formatTime(elapsed), 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(formatTime(trackDurationMs), 40, STATUS_H / 2, 1);

        statusCanvas.fillRect(BAR_X, BAR_Y, BAR_W, BAR_H, T->barBg);
        statusCanvas.drawRect(BAR_X, BAR_Y, BAR_W, BAR_H, rgb(14, 48, 64));

        int fill = (int)(BAR_W * prog);
        if (fill > 3)
        {
            statusCanvas.fillRect(BAR_X, BAR_Y, fill - 2, BAR_H, T->accent1);
            statusCanvas.fillRect(BAR_X + fill - 2, BAR_Y, 2, BAR_H, T->accent2);
        }
        else if (fill > 0)
        {
            statusCanvas.fillRect(BAR_X, BAR_Y, fill, BAR_H, T->accent1);
        }

        char vbuf[8];
        snprintf(vbuf, sizeof(vbuf), "VOL %d", volPct);
        statusCanvas.setTextColor(T->accent3);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "BAT %d%%", batteryLevel);
            statusCanvas.setTextColor(T->textMid);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }

        statusCanvas.drawFastHLine(0, STATUS_H - 1, 5, T->accent2);
        statusCanvas.drawFastVLine(0, STATUS_H - 5, 5, T->accent2);
    }

    else if (themeIdx == 1)
    {
        const int BAR_X = 100, BAR_W = 92, BAR_H = 6;
        const int BAR_Y = (STATUS_H - BAR_H) / 2;

        for (int x = 0; x < SCREEN_W; x += 5)
            statusCanvas.drawFastHLine(x, 0, 3, T->textDim);

        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString("POS>", 2, STATUS_H / 2, 1);

        statusCanvas.setTextColor(T->accent1);
        statusCanvas.drawString(formatTime(elapsed), 26, STATUS_H / 2, 1);

        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString("/", 56, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString(formatTime(trackDurationMs), 62, STATUS_H / 2, 1);

        int segs = BAR_W / 5;
        int litSegs = (int)(segs * prog);
        for (int s = 0; s < segs; s++)
        {
            int bx = BAR_X + s * 5;
            statusCanvas.fillRect(bx, BAR_Y, 4, BAR_H,
                                  s < litSegs ? T->accent1 : T->barBg);
        }

        statusCanvas.setTextDatum(middle_right);
        statusCanvas.setTextColor(T->textDim);
        char vtag[10];
        snprintf(vtag, sizeof(vtag), "VOL:%d", volPct);
        statusCanvas.drawString(vtag, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "B:%d%%", batteryLevel);
            statusCanvas.setTextColor(T->accent2);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }
    }

    else if (themeIdx == 2)
    {
        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);

        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->textBright);
        statusCanvas.drawString(formatTime(elapsed), 4, STATUS_H / 2 - 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString("/" + formatTime(trackDurationMs), 4, STATUS_H / 2 + 6, 1);

        for (int row = 2; row < STATUS_H - 2; row++)
        {
            int dx = (row - 2) / 3;
            statusCanvas.drawPixel(64 + dx, row, T->textDim);
        }

        const int BAR_X = 72, BAR_W = 122, BAR_H = 7;
        const int BAR_Y = (STATUS_H - BAR_H) / 2;
        statusCanvas.fillRect(BAR_X, BAR_Y, BAR_W, BAR_H, T->barBg);
        int fill = (int)(BAR_W * prog);
        if (fill > 0)
            statusCanvas.fillRect(BAR_X, BAR_Y, fill, BAR_H, T->accent1);
        for (int t = 1; t < 10; t++)
        {
            int tx = BAR_X + BAR_W * t / 10;
            statusCanvas.drawFastVLine(tx, BAR_Y + 1, BAR_H - 2,
                                       t * 10 <= (int)(prog * 100) ? T->bg : T->textMid);
        }

        char vbuf[6];
        snprintf(vbuf, sizeof(vbuf), "%d", volPct);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString("VOL", SCREEN_W - 2 - (int)strlen(vbuf) * 6 - 4, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[6];
            snprintf(bbuf, sizeof(bbuf), "%d%%", batteryLevel);
            statusCanvas.setTextColor(T->textBright);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
            statusCanvas.setTextColor(T->textMid);
            statusCanvas.drawString("BAT", SCREEN_W - 2 - (int)strlen(bbuf) * 6 - 4, 13, 1);
        }
        else
        {
            statusCanvas.setTextColor(T->textDim);
            statusCanvas.drawString("BAT --", SCREEN_W - 2, 13, 1);
        }
    }

    else if (themeIdx == 3)
    {
        const int BAR_X = 72, BAR_W = 120, BAR_H = 6;
        const int BAR_Y = (STATUS_H - BAR_H) / 2;

        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->accent2);

        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString(formatTime(elapsed), 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(formatTime(trackDurationMs), 40, STATUS_H / 2, 1);

        statusCanvas.fillRect(BAR_X, BAR_Y, BAR_W, BAR_H, T->barBg);

        int fill = (int)(BAR_W * prog);
        if (fill > 0)
        {
            int half = BAR_H / 2;
            statusCanvas.fillRect(BAR_X, BAR_Y, fill, half, T->accent1);
            statusCanvas.fillRect(BAR_X, BAR_Y + half, fill, BAR_H - half, T->accent2);
        }

        char vbuf[8];
        snprintf(vbuf, sizeof(vbuf), "VOL %d", volPct);
        statusCanvas.setTextColor(T->accent3);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "BAT %d%%", batteryLevel);
            statusCanvas.setTextColor(T->accent2);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }
    }

    else if (themeIdx == 4)
    {
        const int BAR_X = 72, BAR_W = 100, BAR_H = 3;
        const int BAR_Y = (STATUS_H - BAR_H) / 2;

        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);

        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->textBright);
        statusCanvas.drawString(formatTime(elapsed), 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(formatTime(trackDurationMs), 38, STATUS_H / 2, 1);

        statusCanvas.fillRect(BAR_X, BAR_Y, BAR_W, BAR_H, T->barBg);
        int fill = (int)(BAR_W * prog);
        if (fill > 0)
            statusCanvas.fillRect(BAR_X, BAR_Y, fill, BAR_H, T->accent1);

        int maxBlocks = 6;
        int litBlocks = (int)(volPct * maxBlocks / 100.0f + 0.5f);
        int blockW = 4, blockGap = 2;
        int batW = (batteryLevel >= 0) ? 28 : 0;
        int totalW = maxBlocks * (blockW + blockGap) - blockGap;
        int bx0 = SCREEN_W - 2 - batW - totalW;
        for (int b = 0; b < maxBlocks; b++)
        {
            int bx = bx0 + b * (blockW + blockGap);
            uint16_t bc = (b < litBlocks) ? T->accent2 : T->textDim;
            statusCanvas.fillRect(bx, BAR_Y - 1, blockW, BAR_H + 2, bc);
        }
        if (batteryLevel >= 0)
        {
            char bbuf[6];
            snprintf(bbuf, sizeof(bbuf), "%d%%", batteryLevel);
            statusCanvas.setTextColor(T->textDim);
            statusCanvas.setTextDatum(middle_left);
            int sepX = bx0 + totalW + 4;
            statusCanvas.drawString("|", sepX, STATUS_H / 2, 1);
            statusCanvas.drawString(bbuf, sepX + 8, STATUS_H / 2, 1);
        }
    }

    statusCanvas.pushSprite(0, STATUS_Y);
}

void toggleHelp()
{
    helpVisible = !helpVisible;
    if (helpVisible)
        drawHelp();
    else
        drawAll();
}

void drawHelp()
{
    auto &D = M5Cardputer.Display;

    D.fillRect(0, 0, SCREEN_W, SCREEN_H, T->bg);
    D.drawRect(4, 4, SCREEN_W - 8, SCREEN_H - 8, T->accent1);
    D.drawRect(5, 5, SCREEN_W - 10, SCREEN_H - 10, T->textDim);

    struct Row
    {
        const char *key;
        const char *desc;
    };

    if (webRadioMode)
    {
        static const Row rows[] = {
            {"ENTER/SPC", "Play / Stop stream"},
            {"; / .", "Cursor up / down"},
            {"A", "Add radio station"},
            {"X", "Remove station"},
            {"R", "Reconnect stream"},
            {"W / DEL", "Back to music player"},
            {"+ / -", "Volume up / down"},
            {"1-5", "Switch theme"},
            {"O", "Screen on / off"},
            {"M", "Settings menu"},
            {"D", "Debug overlay"},
            {"I", "Force AAC codec"},
            {"H", "Close / Open help"},
        };
        const int rows_n = sizeof(rows) / sizeof(rows[0]);
        const int startY = 8;
        const int rowH = (SCREEN_H - startY * 2) / rows_n;
        for (int i = 0; i < rows_n; i++)
        {
            int y = startY + i * rowH + rowH / 2;
            if (i % 2 == 0)
                D.fillRect(6, startY + i * rowH, SCREEN_W - 12, rowH, T->selRow);
            D.setTextDatum(middle_left);
            D.setTextColor(T->accent2);
            D.drawString(rows[i].key, 12, y, 1);
            D.setTextColor(T->textMid);
            D.drawString(rows[i].desc, 72, y, 1);
        }
    }
    else
    {
        static const Row rows[] = {
            {"ENTER", "Open folder / Play track"},
            {"DEL", "Back to parent folder"},
            {"SPACE", "Pause / Resume"},
            {"; / .", "Cursor up / down"},
            {", / /", "Rewind back / forward"},
            {"+ / -", "Volume up / down"},
            {"W", "Switch to Web Radio"},
            {"R", "Cycle repeat mode"},
            {"S", "Toggle shuffle"},
            {"1-5", "Switch theme"},
            {"O", "Screen on / off"},
            {"M", "Settings menu"},
            {"D", "Debug overlay"},
            {"H", "Close / Open help"},
        };
        const int rows_n = sizeof(rows) / sizeof(rows[0]);
        const int startY = 4;
        const int rowH = (SCREEN_H - startY * 2) / rows_n;
        for (int i = 0; i < rows_n; i++)
        {
            int y = startY + i * rowH + rowH / 2;
            if (i % 2 == 0)
                D.fillRect(6, startY + i * rowH, SCREEN_W - 12, rowH, T->selRow);
            D.setTextDatum(middle_left);
            D.setTextColor(T->accent2);
            D.drawString(rows[i].key, 12, y, 1);
            D.setTextColor(T->textMid);
            D.drawString(rows[i].desc, 68, y, 1);
        }
    }
}

void drawRadioAll()
{
    drawRadioHeader();
    drawRadioList();
    drawRadioStatus();
}

void drawRadioHeader()
{
#define D headerCanvas
    D.fillRect(0, 0, SCREEN_W, HEADER_H, T->hdrBg);

    String wifiStatus = wifiConnected ? wifiSSID : "NOT CONNECTED";
    if (wifiConnected)
    {
        // Coarse signal-quality indicator so a weak connection is visible
        // before the stream actually drops. Uses the same RSSI thresholds
        // as the debug overlay.
        int rssi = WiFi.RSSI();
        const char *sig = (rssi >= -55)   ? "||||"
                           : (rssi >= -65) ? "|||."
                           : (rssi >= -75) ? "||.."
                           : (rssi >= -85) ? "|..."
                                           : "....";
        wifiStatus += " ";
        wifiStatus += sig;
    }
    if ((int)wifiStatus.length() > 20)
        wifiStatus = wifiStatus.substring(0, 19) + ">";

    String stationLine = "-- SELECT STATION --";
    if (radioIsPlaying && radioPlaying >= 0 && radioPlaying < radioCount)
        stationLine = radioList[radioPlaying].name;
    if ((int)stationLine.length() > 24)
        stationLine = stationLine.substring(0, 23) + ">";

    if (themeIdx == 0)
    {
        D.fillRect(0, 0, 3, HEADER_H, T->accent1);
        D.setTextDatum(middle_left);
        D.setTextColor(radioIsPlaying ? T->accent1 : T->textDim);
        D.drawString(radioIsPlaying ? ">> LIVE" : "-  RADIO", 8, 7, 1);
        D.setTextColor(wifiConnected ? T->accent3 : T->accent1);
        D.setTextDatum(middle_right);
        D.drawString(wifiStatus, SCREEN_W - 4, 7, 1);
        D.setTextColor(T->accent2);
        D.setTextDatum(middle_left);
        D.drawString(stationLine, 8, 22, 2);
        D.drawFastHLine(3, HEADER_H - 2, SCREEN_W - 3, T->accent1);
        D.drawFastHLine(3, HEADER_H - 1, SCREEN_W - 3, T->textDim);
        D.drawFastHLine(0, 0, 6, T->accent1);
        D.drawFastVLine(0, 0, 6, T->accent1);
        D.drawFastHLine(SCREEN_W - 6, 0, 6, T->accent1);
        D.drawFastVLine(SCREEN_W - 1, 0, 6, T->accent1);
    }
    else if (themeIdx == 1)
    {
        D.setTextDatum(middle_left);
        D.setTextColor(T->textDim);
        D.drawString("BRKN_SIGNAL // RADIO", 4, 7, 1);
        D.setTextColor(radioIsPlaying ? T->accent1 : T->textDim);
        D.setTextDatum(middle_right);
        D.drawString(wifiConnected ? wifiStatus : "NO WIFI", SCREEN_W - 4, 7, 1);
        const int CUR_X = 4, CUR_W = 5, CUR_GAP = 3;
        D.fillRect(CUR_X, 16, CUR_W, 10, (radioIsPlaying && cursorVisible) ? T->accent1 : T->hdrBg);
        D.setTextColor(radioIsPlaying ? T->accent1 : T->textMid);
        D.setTextDatum(middle_left);
        D.drawString(stationLine, CUR_X + CUR_W + CUR_GAP, 22, 2);
        for (int x = 0; x < SCREEN_W; x += 5)
            D.drawFastHLine(x, HEADER_H - 1, 3, T->textDim);
    }
    else
    {
        D.fillRect(0, 0, 3, HEADER_H, T->accent1);
        D.drawFastVLine(0, 0, HEADER_H / 2, T->accent2);
        D.setTextDatum(middle_left);
        D.setTextColor(radioIsPlaying ? T->accent1 : T->textDim);
        D.drawString(radioIsPlaying ? "* ON AIR" : "* WEB RADIO", 8, 7, 1);
        D.setTextColor(wifiConnected ? T->accent3 : T->textDim);
        D.setTextDatum(middle_right);
        D.drawString(wifiStatus, SCREEN_W - 4, 7, 1);
        D.setTextColor(T->textBright);
        D.setTextDatum(middle_left);
        D.drawString(stationLine, 8, 22, 2);
        D.drawFastHLine(3, HEADER_H - 1, SCREEN_W - 3, T->textDim);
        if (themeIdx == 3)
        {
            D.drawFastHLine(0, HEADER_H - 2, SCREEN_W, T->accent2);
            D.drawFastHLine(0, HEADER_H - 1, SCREEN_W, rgb(0, 80, 70));
        }
        else if (themeIdx == 4)
        {
            D.drawFastHLine(0, HEADER_H - 1, SCREEN_W, T->accent1);
        }
    }

    if (hdrMsgEnd > 0 && millis() < hdrMsgEnd)
    {
        D.setTextDatum(middle_right);
        D.setTextColor(T->accent2);
        D.drawString(hdrMsg, SCREEN_W - 4, 22, 1);
    }

    headerCanvas.pushSprite(0, 0);
#undef D
}

void drawRadioRow(int idx)
{
    if (idx < radioScrollTop || idx >= radioScrollTop + VISIBLE_TRACKS)
        return;
    if (idx < 0 || idx >= radioCount)
        return;

    auto &D = M5Cardputer.Display;
    int sbW = (themeIdx == 1) ? 7 : 3;
    int i = idx - radioScrollTop;
    int y = LIST_Y + i * LIST_ITEM_H;
    int midY = y + LIST_ITEM_H / 2;
    bool sel = (idx == radioSelected);
    bool play = (idx == radioPlaying && radioIsPlaying);

    int rowW = SCREEN_W - sbW;
    if (themeIdx == 0)
    {
        if (sel)
        {
            D.fillRect(0, y, rowW / 2, LIST_ITEM_H, T->selRow);
            D.fillRect(rowW / 2, y, rowW - rowW / 2, LIST_ITEM_H, T->bg);
        }
        else
        {
            D.fillRect(0, y, rowW, LIST_ITEM_H, T->bg);
        }
        D.drawFastHLine(0, y + LIST_ITEM_H - 1, rowW, rgb(9, 21, 32));
    }
    else if (themeIdx == 1)
    {
        D.fillRect(0, y, rowW, LIST_ITEM_H, sel ? T->selRow : T->bg);
    }
    else
    {
        if (sel)
        {
            D.fillRect(0, y, 80, LIST_ITEM_H, rgb(20, 16, 10));
            D.fillRect(80, y, rowW - 80, LIST_ITEM_H, T->bg);
        }
        else
        {
            D.fillRect(0, y, rowW, LIST_ITEM_H, T->bg);
        }
        D.drawFastHLine(0, y + LIST_ITEM_H - 1, rowW, T->textDim);
    }

    if (themeIdx == 0 || themeIdx == 3)
    {
        if (play)
            D.fillRect(3, y, 3, LIST_ITEM_H, T->accent1);
        else if (sel)
            D.fillRect(3, y, 2, LIST_ITEM_H, T->accent2);
    }
    else if (themeIdx == 2)
    {
        if (play)
            D.fillRect(3, y + 3, 3, LIST_ITEM_H - 6, T->accent1);
        if (sel)
        {
            D.drawFastHLine(SCREEN_W - sbW - 7, y + 3, 5, T->accent2);
            D.drawFastVLine(SCREEN_W - sbW - 3, y + 3, 5, T->accent2);
        }
    }
    else if (themeIdx == 4)
    {
        if (sel)
            D.drawFastVLine(0, y, LIST_ITEM_H, T->accent1);
        if (play)
            D.drawFastVLine(1, y, LIST_ITEM_H, T->accent2);
    }

    char numBuf[4];
    snprintf(numBuf, sizeof(numBuf), "%02d", idx + 1);
    if (themeIdx == 1)
    {
        D.setTextDatum(middle_left);
        if (play)
        {
            D.setTextColor(T->accent1);
            D.drawString(">", 2, midY, 1);
        }
        else if (sel)
        {
            D.setTextColor(T->accent2);
            D.drawString("|", 2, midY, 1);
        }
        D.setTextColor(T->textDim);
        D.drawString(numBuf, 12, midY, 1);
    }
    else
    {
        uint16_t nc = (themeIdx == 4) ? T->textDim
                      : play          ? T->accent1
                                      : (sel ? T->accent2 : T->textDim);
        D.setTextColor(nc);
        D.setTextDatum(middle_left);
        D.drawString(numBuf, 8, midY, 1);
    }

    int nameX = (themeIdx == 1) ? 30 : 26;
    String lbl = radioList[idx].name;
    int maxChars = (themeIdx == 1) ? 31 : 30;
    if ((int)lbl.length() > maxChars)
        lbl = lbl.substring(0, maxChars - 1) + ">";
    uint16_t nc;
    if (themeIdx == 1)
        nc = play ? T->accent1 : (sel ? T->accent2 : T->textMid);
    else
        nc = play ? T->accent1 : (sel ? T->textBright : T->textMid);
    D.setTextColor(nc);
    D.setTextDatum(middle_left);
    D.drawString(lbl, nameX, midY, 1);

    if (themeIdx == 0 && play)
    {
        uint16_t glowCol = rgb(80, 0, 40);
        D.drawFastHLine(0, y, rowW, glowCol);
        D.drawFastHLine(0, y + LIST_ITEM_H - 1, rowW, glowCol);
    }
    if (themeIdx == 1 && sel)
    {
        uint16_t cur = cursorVisible ? T->accent1 : T->bg;
        D.fillRect(SCREEN_W - sbW - 8, y + 3, 5, LIST_ITEM_H - 6, cur);
    }
}

void drawRadioList()
{
    auto &D = M5Cardputer.Display;
    int sbW = (themeIdx == 1) ? 7 : 3;

    D.drawFastHLine(0, HEADER_H, SCREEN_W, T->bg);
    D.drawFastHLine(0, STATUS_Y - 1, SCREEN_W, T->bg);

    if (radioCount == 0)
    {
        D.fillRect(0, LIST_Y, SCREEN_W, VISIBLE_TRACKS * LIST_ITEM_H, T->bg);
        D.setTextDatum(middle_center);
        D.setTextColor(T->textDim);
        D.drawString("No stations saved.", SCREEN_W / 2, LIST_Y + 28, 1);
        D.drawString("Press A to add one.", SCREEN_W / 2, LIST_Y + 42, 1);
        return;
    }

    int visCount = min(VISIBLE_TRACKS, radioCount - radioScrollTop);
    for (int i = 0; i < visCount; i++)
        drawRadioRow(radioScrollTop + i);
    for (int i = visCount; i < VISIBLE_TRACKS; i++)
        D.fillRect(0, LIST_Y + i * LIST_ITEM_H, SCREEN_W, LIST_ITEM_H, T->bg);

    if (radioCount > VISIBLE_TRACKS)
    {
        int listH = VISIBLE_TRACKS * LIST_ITEM_H;
        int thH = max(5, listH * VISIBLE_TRACKS / radioCount);
        int thY = LIST_Y + (listH - thH) * radioScrollTop / max(1, radioCount - VISIBLE_TRACKS);
        int sbX = SCREEN_W - sbW;
        if (themeIdx == 1)
        {
            D.fillRect(sbX, LIST_Y, sbW, listH, T->bg);
            D.drawFastVLine(sbX, LIST_Y, listH, T->textDim);
            D.fillRect(sbX + 1, thY, sbW - 1, thH, T->textDim);
            D.drawRect(sbX + 1, thY, sbW - 1, thH, T->textMid);
        }
        else
        {
            D.fillRect(sbX, LIST_Y, sbW, listH, T->barBg);
            D.fillRect(sbX, thY, sbW, thH, themeIdx == 2 ? T->accent2 : T->accent1);
        }
    }
}

void drawRadioStatus()
{
    int volPct = (volume * 100) / 255;
    statusCanvas.fillSprite(T->hdrBg);

    const char *streamLabel = radioIsPlaying ? "LIVE" : "IDLE";
    uint16_t streamCol = radioIsPlaying ? T->accent1 : T->textDim;

    if (themeIdx == 0)
    {
        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);
        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("A", 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(":ADD", 10, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("X", 38, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(":RMV", 44, STATUS_H / 2, 1);
        statusCanvas.setTextDatum(middle_center);
        statusCanvas.setTextColor(streamCol);
        statusCanvas.drawString(streamLabel, SCREEN_W / 2, STATUS_H / 2, 1);
        char vbuf[8];
        snprintf(vbuf, sizeof(vbuf), "VOL %d", volPct);
        statusCanvas.setTextColor(T->accent3);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "BAT %d%%", batteryLevel);
            statusCanvas.setTextColor(T->textMid);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }
        statusCanvas.drawFastHLine(0, STATUS_H - 1, 5, T->accent2);
        statusCanvas.drawFastVLine(0, STATUS_H - 5, 5, T->accent2);
    }
    else if (themeIdx == 1)
    {
        for (int x = 0; x < SCREEN_W; x += 5)
            statusCanvas.drawFastHLine(x, 0, 3, T->textDim);
        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString("[A]ADD [X]RMV", 2, STATUS_H / 2, 1);
        statusCanvas.setTextColor(streamCol);
        statusCanvas.setTextDatum(middle_center);
        statusCanvas.drawString(streamLabel, SCREEN_W / 2, STATUS_H / 2, 1);
        char vtag[10];
        snprintf(vtag, sizeof(vtag), "VOL:%d", volPct);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(vtag, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "B:%d%%", batteryLevel);
            statusCanvas.setTextColor(T->accent2);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }
    }
    else if (themeIdx == 2)
    {
        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);
        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("A", 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString(":ADD", 10, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("X", 40, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString(":RMV", 46, STATUS_H / 2, 1);
        statusCanvas.setTextColor(streamCol);
        statusCanvas.setTextDatum(middle_center);
        statusCanvas.drawString(streamLabel, SCREEN_W / 2, STATUS_H / 2, 1);
        char vbuf[6];
        snprintf(vbuf, sizeof(vbuf), "%d", volPct);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString("VOL", SCREEN_W - 2 - (int)strlen(vbuf) * 6 - 4, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[6];
            snprintf(bbuf, sizeof(bbuf), "%d%%", batteryLevel);
            statusCanvas.setTextColor(T->textBright);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
            statusCanvas.setTextColor(T->textMid);
            statusCanvas.drawString("BAT", SCREEN_W - 2 - (int)strlen(bbuf) * 6 - 4, 13, 1);
        }
    }
    else if (themeIdx == 3)
    {
        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->accent2);
        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("A", 4, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(":ADD", 10, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->accent2);
        statusCanvas.drawString("X", 38, STATUS_H / 2, 1);
        statusCanvas.setTextColor(T->textDim);
        statusCanvas.drawString(":RMV", 44, STATUS_H / 2, 1);
        statusCanvas.setTextColor(streamCol);
        statusCanvas.setTextDatum(middle_center);
        statusCanvas.drawString(streamLabel, SCREEN_W / 2, STATUS_H / 2, 1);
        char vbuf[8];
        snprintf(vbuf, sizeof(vbuf), "VOL %d", volPct);
        statusCanvas.setTextColor(T->accent3);
        statusCanvas.setTextDatum(middle_right);
        statusCanvas.drawString(vbuf, SCREEN_W - 2, 5, 1);
        if (batteryLevel >= 0)
        {
            char bbuf[8];
            snprintf(bbuf, sizeof(bbuf), "BAT %d%%", batteryLevel);
            statusCanvas.setTextColor(T->accent2);
            statusCanvas.drawString(bbuf, SCREEN_W - 2, 13, 1);
        }
    }
    else
    {
        statusCanvas.drawFastHLine(0, 0, SCREEN_W, T->textDim);
        statusCanvas.setTextDatum(middle_left);
        statusCanvas.setTextColor(T->textMid);
        statusCanvas.drawString("A:ADD", 4, STATUS_H / 2, 1);
        statusCanvas.drawString("X:RMV", 38, STATUS_H / 2, 1);
        statusCanvas.setTextColor(streamCol);
        statusCanvas.setTextDatum(middle_center);
        statusCanvas.drawString(streamLabel, SCREEN_W / 2, STATUS_H / 2, 1);
        int litBlocks = (int)(volPct * 6 / 100.0f + 0.5f);
        int batW = (batteryLevel >= 0) ? 28 : 0;
        int bx0 = SCREEN_W - 2 - batW - (6 * 6 - 2);
        for (int b = 0; b < 6; b++)
        {
            int bx = bx0 + b * 6;
            statusCanvas.fillRect(bx, (STATUS_H - 5) / 2, 4, 5, b < litBlocks ? T->accent2 : T->textDim);
        }
        if (batteryLevel >= 0)
        {
            char bbuf[6];
            snprintf(bbuf, sizeof(bbuf), "%d%%", batteryLevel);
            statusCanvas.setTextColor(T->textDim);
            statusCanvas.setTextDatum(middle_left);
            int sepX = bx0 + 34 + 4;
            statusCanvas.drawString("|", sepX, STATUS_H / 2, 1);
            statusCanvas.drawString(bbuf, sepX + 8, STATUS_H / 2, 1);
        }
    }

    statusCanvas.pushSprite(0, STATUS_Y);
}

void drawOverlayFrame(const char *title)
{
    auto &D = M5Cardputer.Display;
    D.fillRect(0, 0, SCREEN_W, SCREEN_H, T->bg);
    D.drawRect(3, 3, SCREEN_W - 6, SCREEN_H - 6, T->accent1);
    D.drawRect(4, 4, SCREEN_W - 8, SCREEN_H - 8, T->textDim);
    D.fillRect(4, 4, SCREEN_W - 8, 18, T->hdrBg);
    D.setTextDatum(middle_left);
    D.setTextColor(T->accent1);
    D.drawString(title, 10, 13, 1);
}

void drawWifiOverlay()
{
    auto &D = M5Cardputer.Display;
    drawOverlayFrame("SELECT WIFI");

    const int LIST_START = 24;
    const int ROW_H = 14;
    const int ROWS_VIS = 6;
    const int FOOTER_Y = 121;

    D.setTextDatum(middle_right);
    D.setTextColor(T->textDim);
    D.drawString("SCAN=S  DEL=cancel", SCREEN_W - 6, 13, 1);

    if (wifiNetCount == 0)
    {
        D.setTextDatum(middle_center);
        D.setTextColor(T->textDim);
        D.drawString("No networks found", SCREEN_W / 2, 68, 1);
        D.drawString("Press S to rescan", SCREEN_W / 2, 82, 1);
    }
    else
    {
        for (int i = 0; i < ROWS_VIS; i++)
        {
            int idx = wifiNetScroll + i;
            if (idx >= wifiNetCount)
                break;
            int y = LIST_START + i * ROW_H;
            bool sel = (idx == wifiNetSel);
            D.fillRect(5, y, SCREEN_W - 10, ROW_H, sel ? T->selRow : T->bg);
            if (sel)
                D.fillRect(5, y, 3, ROW_H, T->accent1);

            String ssid = wifiNets[idx].ssid;
            if ((int)ssid.length() > 24)
                ssid = ssid.substring(0, 23) + ">";
            D.setTextDatum(middle_left);
            D.setTextColor(sel ? T->textBright : T->textMid);
            D.drawString(ssid, 12, y + ROW_H / 2, 1);

            if (wifiNets[idx].encrypted)
            {
                D.setTextColor(sel ? T->accent3 : T->textDim);
                D.drawString("*", SCREEN_W - 14, y + ROW_H / 2, 1);
            }
        }
        if (wifiNetCount > ROWS_VIS)
        {
            int sbH = ROWS_VIS * ROW_H;
            int thH = max(5, sbH * ROWS_VIS / wifiNetCount);
            int thY = LIST_START + (sbH - thH) * wifiNetScroll / max(1, wifiNetCount - ROWS_VIS);
            D.fillRect(SCREEN_W - 8, LIST_START, 3, sbH, T->barBg);
            D.fillRect(SCREEN_W - 8, thY, 3, thH, T->accent2);
        }
    }
    D.setTextDatum(middle_center);
    D.setTextColor(T->textDim);
    D.drawString("ENTER=connect", SCREEN_W / 2, FOOTER_Y, 1);
}

void drawWifiPassOverlay(bool inputOnly)
{
    auto &D = M5Cardputer.Display;
    if (!inputOnly)
    {
        drawOverlayFrame("WIFI PASSWORD");
        D.setTextDatum(middle_left);
        D.setTextColor(T->textMid);
        String ssidLine = "Net: " + inputSaved;
        if ((int)ssidLine.length() > 32)
            ssidLine = ssidLine.substring(0, 31) + ">";
        D.drawString(ssidLine, 8, 34, 1);
        D.drawFastHLine(6, 42, SCREEN_W - 12, T->textDim);
        D.setTextColor(T->textDim);
        D.setTextDatum(middle_center);
        D.drawString("ENTER=connect   DEL=back", SCREEN_W / 2, 100, 1);
        D.fillRect(6, 52, SCREEN_W - 12, 18, T->hdrBg);
        D.drawRect(6, 52, SCREEN_W - 12, 18, T->accent2);
    }
    String disp = String(inputBuf) + "_";
    if ((int)disp.length() > 30)
        disp = disp.substring((int)disp.length() - 30);
    while ((int)disp.length() < 30)
        disp += ' ';
    D.setTextDatum(middle_left);
    D.setTextColor(T->textBright, T->hdrBg);
    D.drawString(disp, 10, 61, 1);
    D.setTextColor(T->textBright);
}

void drawAddUrlOverlay(bool inputOnly)
{
    auto &D = M5Cardputer.Display;
    if (!inputOnly)
    {
        drawOverlayFrame("ADD RADIO STATION");
        D.setTextDatum(middle_left);
        D.setTextColor(T->textMid);
        D.drawString("Enter stream URL:", 8, 34, 1);
        D.setTextColor(T->textDim);
        D.setTextDatum(middle_center);
        D.drawString("Tip: http:// stream URL", SCREEN_W / 2, 80, 1);
        D.drawString("ENTER=next   DEL=backspace", SCREEN_W / 2, 100, 1);
        D.drawString("DEL on empty=cancel", SCREEN_W / 2, 112, 1);
        D.fillRect(6, 44, SCREEN_W - 12, 18, T->hdrBg);
        D.drawRect(6, 44, SCREEN_W - 12, 18, T->accent2);
    }
    String disp = String(inputBuf) + "_";
    if ((int)disp.length() > 30)
        disp = disp.substring((int)disp.length() - 30);
    while ((int)disp.length() < 30)
        disp += ' ';
    D.setTextDatum(middle_left);
    D.setTextColor(T->textBright, T->hdrBg);
    D.drawString(disp, 10, 53, 1);
    D.setTextColor(T->textBright);
}

void drawAddNameOverlay(bool inputOnly)
{
    auto &D = M5Cardputer.Display;
    if (!inputOnly)
    {
        drawOverlayFrame("STATION NAME");
        D.setTextDatum(middle_left);
        D.setTextColor(T->textMid);
        D.drawString("Name (or ENTER to accept):", 8, 34, 1);
        D.setTextColor(T->textDim);
        D.setTextDatum(middle_center);
        D.drawString("ENTER=add station   DEL=backspace", SCREEN_W / 2, 100, 1);
        D.fillRect(6, 44, SCREEN_W - 12, 18, T->hdrBg);
        D.drawRect(6, 44, SCREEN_W - 12, 18, T->accent2);
    }
    String disp = String(inputBuf) + "_";
    if ((int)disp.length() > 30)
        disp = disp.substring((int)disp.length() - 30);
    while ((int)disp.length() < 30)
        disp += ' ';
    D.setTextDatum(middle_left);
    D.setTextColor(T->textBright, T->hdrBg);
    D.drawString(disp, 10, 53, 1);
    D.setTextColor(T->textBright);
}

void drawRemoveConfirm()
{
    auto &D = M5Cardputer.Display;
    drawOverlayFrame("REMOVE STATION?");

    String name = (radioCount > 0 && radioSelected < radioCount)
                      ? radioList[radioSelected].name
                      : "?";
    if ((int)name.length() > 28)
        name = name.substring(0, 27) + ">";

    D.setTextDatum(middle_center);
    D.setTextColor(T->textBright);
    D.drawString(name, SCREEN_W / 2, 55, 1);
    D.setTextColor(T->textDim);
    D.drawString("This will be deleted.", SCREEN_W / 2, 75, 1);
    D.setTextColor(T->accent1);
    D.drawString("ENTER=remove", SCREEN_W / 2, 100, 1);
    D.setTextColor(T->textMid);
    D.drawString("DEL=cancel", SCREEN_W / 2, 114, 1);
}

// ---------------------------------------------------------------------------
// Settings menu (opened with 'm' in music and radio modes)
// ---------------------------------------------------------------------------
static void adjustSetting(int sel, int dir)
{
    if (sel == 0)
    {
        static const uint8_t opts[] = {5, 10, 15, 20, 30, 45, 60};
        const int n = sizeof(opts) / sizeof(opts[0]);
        int idx = 0;
        for (int i = 0; i < n; i++)
            if (opts[i] == seekSeconds)
            {
                idx = i;
                break;
            }
        idx = (idx + dir + n) % n;
        seekSeconds = opts[idx];
    }
    else if (sel == 1)
    {
        wifiPowerSave = !wifiPowerSave;
        applyWifiPowerSave();
    }
    else if (sel == 2)
    {
        static const uint8_t opts[] = {16, 64, 128, 200, 255};
        const int n = sizeof(opts) / sizeof(opts[0]);
        int idx = 0;
        for (int i = 0; i < n; i++)
            if (opts[i] == screenBrightness)
            {
                idx = i;
                break;
            }
        idx = (idx + dir + n) % n;
        screenBrightness = opts[idx];
        if (screenOn)
            M5Cardputer.Display.setBrightness(screenBrightness);
    }
    else if (sel == 3)
    {
        static const uint16_t opts[] = {0, 15, 30, 60, 120};
        const int n = sizeof(opts) / sizeof(opts[0]);
        int idx = 0;
        for (int i = 0; i < n; i++)
            if (opts[i] == autoScreenOffSec)
            {
                idx = i;
                break;
            }
        idx = (idx + dir + n) % n;
        autoScreenOffSec = opts[idx];
    }
    settingsDirty = true;
    settingsDirtyMs = millis();
}

void drawSettingsMenu()
{
    auto &D = M5Cardputer.Display;
    drawOverlayFrame("SETTINGS");

    D.setTextDatum(middle_right);
    D.setTextColor(T->textDim);
    D.drawString("DEL=exit  ;/.=sel  +/-=chg", SCREEN_W - 6, 13, 1);

    const int NROWS = 4;
    const char *labels[NROWS] = {
        "Seek step",
        "WiFi power save",
        "Brightness",
        "Auto screen off"};
    String values[NROWS];
    values[0] = String(seekSeconds) + "s";
    values[1] = wifiPowerSave ? "ON" : "OFF";
    values[2] = String(screenBrightness);
    values[3] = (autoScreenOffSec == 0) ? String("OFF") : (String(autoScreenOffSec) + "s");

    const int listY = 30;
    const int rowH = 20;
    for (int i = 0; i < NROWS; i++)
    {
        int y = listY + i * rowH;
        bool sel = (i == settingsSel);
        if (sel)
            D.fillRect(5, y, SCREEN_W - 10, rowH, T->selRow);
        D.setTextDatum(middle_left);
        D.setTextColor(sel ? T->accent2 : T->textMid);
        D.drawString(labels[i], 12, y + rowH / 2, 1);
        D.setTextDatum(middle_right);
        D.setTextColor(sel ? T->accent1 : T->textBright);
        D.drawString(values[i], SCREEN_W - 12, y + rowH / 2, 1);
    }

    D.setTextDatum(middle_center);
    D.setTextColor(T->textDim);
    D.drawString("ENTER=toggle  M/DEL=close", SCREEN_W / 2, SCREEN_H - 8, 1);
}

void enterSettingsMenu()
{
    settingsMenuVisible = true;
    settingsSel = 0;
    drawSettingsMenu();
}

void exitSettingsMenu()
{
    settingsMenuVisible = false;
    saveSettings();
    settingsDirty = false;
    if (webRadioMode)
        drawRadioAll();
    else
        drawAll();
}

void handleSettingsInput(Keyboard_Class::KeysState &ks)
{
    const int NSETTINGS = 4;
    if (ks.del)
    {
        exitSettingsMenu();
        return;
    }
    bool changed = false;
    for (auto c : ks.word)
    {
        switch (c)
        {
        case 'm':
        case 'M':
            exitSettingsMenu();
            return;
        case ';':
            settingsSel = (settingsSel - 1 + NSETTINGS) % NSETTINGS;
            changed = true;
            break;
        case '.':
            settingsSel = (settingsSel + 1) % NSETTINGS;
            changed = true;
            break;
        case '+':
        case '=':
            adjustSetting(settingsSel, +1);
            changed = true;
            break;
        case '-':
            adjustSetting(settingsSel, -1);
            changed = true;
            break;
        }
    }
    if (ks.enter)
    {
        if (settingsSel == 1)
        {
            wifiPowerSave = !wifiPowerSave;
            applyWifiPowerSave();
            settingsDirty = true;
            settingsDirtyMs = millis();
        }
        changed = true;
    }
    if (changed)
        drawSettingsMenu();
}

// ---------------------------------------------------------------------------
// Debug overlay (toggle with 'd' in music and radio modes)
// Live system diagnostics: heap, min-heap, radio buffer fill, codec, RSSI,
// uptime, battery. The main loop refreshes it every 500ms while open.
// ---------------------------------------------------------------------------
void drawDebug()
{
    auto &D = M5Cardputer.Display;
    drawOverlayFrame("DEBUG");

    const int y0 = 30;
    const int rowH = 14;
    auto row = [&](int i, const char *label, const String &val)
    {
        int y = y0 + i * rowH;
        D.setTextDatum(middle_left);
        D.setTextColor(T->textDim);
        D.drawString(label, 10, y, 1);
        D.setTextColor(T->accent2);
        D.setTextDatum(middle_right);
        D.drawString(val, SCREEN_W - 10, y, 1);
    };

    String mode = webRadioMode ? "RADIO" : "MUSIC";
    row(0, "MODE", mode + "  T" + String(themeIdx));

    uint32_t freeH = ESP.getFreeHeap();
    uint32_t minH = ESP.getMinFreeHeap();
    row(1, "HEAP FREE", String(freeH / 1024) + "K");
    row(2, "HEAP MIN", String(minH / 1024) + "K");

    if (webRadioMode)
    {
        String codec = radioMp3 ? "MP3" : (aac ? "AAC" : "--");
        row(3, "CODEC", codec);
        if (radioBuf)
        {
            uint32_t fill = radioBuf->getFillLevel();
            int pct = (int)(fill * 100 / RADIO_HTTP_BUF);
            char b[20];
            snprintf(b, sizeof(b), "%luK/%dK %d%%", (unsigned long)(fill / 1024),
                     (int)(RADIO_HTTP_BUF / 1024), pct);
            row(4, "NET BUFFER", String(b));
        }
        else
        {
            row(4, "NET BUFFER", "--");
        }
        if (wifiConnected)
        {
            int rssi = WiFi.RSSI();
            char r[16];
            snprintf(r, sizeof(r), "%d dBm", rssi);
            row(5, "RSSI", String(r));
        }
        else
        {
            row(5, "WIFI", "OFFLINE");
        }
    }
    else
    {
        row(3, "CODEC", mp3 ? "MP3" : (aac ? "AAC/M4A" : "--"));
        String st = isPlaying ? "PLAY" : (isPaused ? "PAUSE" : "STOP");
        row(4, "STATE", st);
        int n = (int)items.size();
        row(5, "ITEMS", String(n));
    }

    unsigned long up = millis() / 1000;
    char upStr[12];
    snprintf(upStr, sizeof(upStr), "%02lu:%02lu:%02lu", up / 3600, (up / 60) % 60, up % 60);
    row(6, "UPTIME", String(upStr));

    String bat = (batteryLevel >= 0) ? (String(batteryLevel) + "%") : "--";
    row(7, "BATTERY", bat);

    D.setTextDatum(middle_center);
    D.setTextColor(T->textDim);
    D.drawString("d=close", SCREEN_W / 2, SCREEN_H - 8, 1);
}

void toggleDebug()
{
    debugOverlayVisible = !debugOverlayVisible;
    if (debugOverlayVisible)
        drawDebug();
    else if (webRadioMode)
        drawRadioAll();
    else
        drawAll();
}