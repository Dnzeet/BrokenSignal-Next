#pragma once
#include "State.h"
void loadRadioList();
void saveRadioList();
bool loadWifiConfig(String &ssid, String &pass);
void saveWifiConfig(const String &ssid, const String &pass);
String generateRadioName(const String &url, int n);
void purgeRadioMemory();
void startRadioStream(int idx);
void stopRadioStream();
void pumpRadioAudio();
void radioScrollEnsureVisible();
void scanWifiNetworks();
bool connectWifi(const String &ssid, const String &pass);
void showWifiOverlay();
void showWifiPassOverlay(const String &ssid);
void showAddUrlOverlay();
void showAddNameOverlay(const String &defaultName);
void showRemoveConfirm();
void handleRadioInput(Keyboard_Class::KeysState &ks);
void handleOverlayInput(Keyboard_Class::KeysState &ks);
void enterWebRadioMode();
void exitWebRadioMode();
void applyWifiPowerSave();