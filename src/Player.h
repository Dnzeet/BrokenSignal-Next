#pragma once
#include "State.h"
unsigned long readM4ADuration(const char *path);
unsigned long readMP3Duration(const char *path, size_t fileSize);
void startTrack(int idx);
void stopAudio();
void pauseAudio();
void resumeAudio();
void pumpAudio();
int pickNextTrack();
unsigned long estimateDuration(int idx);
void saveSettings();
void loadSettings();
void seekTrack(int delta_ms);