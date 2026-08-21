#pragma once
#include <Arduino.h>

// SD Card pins
#define SD_CS 12
#define SD_MOSI 14
#define SD_CLK 40
#define SD_MISO 39

// Screen layout
#define SCREEN_W 240
#define SCREEN_H 135
#define HEADER_H 32
#define LIST_Y 33
#define LIST_ITEM_H 14
#define VISIBLE_TRACKS 6
#define STATUS_H 17
#define STATUS_Y (SCREEN_H - STATUS_H)

// Memory limits
#define RECENT_MAX 10
#define SCAN_CACHE_MAX 11
#define PAGE_SIZE 25
#define NAME_CACHE_MAX 200

// Web Radio
#define RADIO_MAX 20
#define RADIO_HTTP_BUF 24576
#define WIFI_TIMEOUT 15000
#define WIFI_SCAN_MAX 10
#define RADIO_INPUT_MAX 200

// Pre-buffer this many bytes into RADIO_HTTP_BUF before starting the codec,
// so playback doesn't start decoding faster than the network can deliver.
// Capped at RADIO_PREBUFFER_MAX_WAIT_MS in case the stream is slow/dead -
// better to start with a partial buffer than hang the UI.
#define RADIO_PREBUFFER_TARGET (14 * 1024)
#define RADIO_PREBUFFER_MAX_WAIT_MS 1500

// Low-level stream reconnect (inside AudioFileSourceHTTPSStream, triggers on a
// dropped TCP connection mid-read)
#define STREAM_RECONNECT_TRIES 3
#define STREAM_RECONNECT_DELAY_MS 600

// App-level auto-reconnect (triggers when the whole audio generator gives up,
// e.g. the station closed the connection). Uses exponential backoff capped at
// 16s: 1s, 2s, 4s, 8s, 16s, 16s...
#define RADIO_AUTO_RECONNECT_MAX 5

// Compile-time serial debug logging (USB CDC). 0 = off, 1 = log radio events.
#define DEBUG_SERIAL 0

// Battery
#define BATTERY_INTERVAL (2UL * 60 * 1000)

// Settings persistence (settings.cfg)
// Fast path: flush 2s after the last change, but only when idle (see main.cpp)
// Safety net: force-flush even mid-playback if a change has sat unsaved this
// long, so a crash/power loss during a long listening session doesn't wipe it
#define SETTINGS_IDLE_SAVE_DELAY_MS 2000
#define SETTINGS_FORCE_SAVE_INTERVAL_MS (5UL * 60 * 1000)

// Settings (persisted in settings.cfg)
#define SEEK_SECONDS_DEFAULT 10
#define SCREEN_BRIGHTNESS_DEFAULT 128
#define AUTO_SCREEN_OFF_DEFAULT 0