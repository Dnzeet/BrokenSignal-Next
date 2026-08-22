#pragma once
#include <Arduino.h>

static inline uint16_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint16_t)(r >> 3) << 11) | ((uint16_t)(g >> 2) << 5) | (b >> 3);
}

struct Theme
{
    uint16_t bg, hdrBg, accent1, accent2, accent3, textBright, textMid, textDim, barBg, selRow;
    const char *name;
};

static const Theme T_NEON = {
    rgb(2, 4, 8),       // bg
    rgb(5, 14, 24),     // hdrBg
    rgb(255, 45, 120),  // accent1
    rgb(0, 245, 255),   // accent2
    rgb(245, 230, 66),  // accent3
    rgb(200, 234, 245), // textBright
    rgb(58, 106, 122),  // textMid
    rgb(14, 48, 64),    // textDim
    rgb(9, 21, 32),     // barBg
    rgb(10, 30, 46),    // selRow
    "NEON NOIR"};

static const Theme T_TERM = {
    rgb(0, 4, 0),       // bg
    rgb(0, 12, 0),      // hdrBg
    rgb(0, 255, 65),    // accent1
    rgb(255, 145, 0),   // accent2)
    rgb(0, 255, 190),   // accent3s
    rgb(190, 255, 200), // textBright
    rgb(0, 195, 55),    // textMide
    rgb(0, 70, 15),     // textDims
    rgb(0, 16, 2),      // barBgh
    rgb(0, 28, 6),      // selRow
    "GLITCH TERMINAL"};

static const Theme T_CORP = {
    rgb(8, 10, 15),     // bg
    rgb(12, 15, 24),    // hdrBg
    rgb(200, 168, 75),  // accent1
    rgb(232, 201, 106), // accent2
    rgb(200, 168, 75),  // accent3
    rgb(200, 207, 224), // textBright
    rgb(58, 74, 106),   // textMid
    rgb(28, 34, 53),    // textDim
    rgb(28, 34, 53),    // barBg
    rgb(14, 18, 30),    // selRow
    "CORPO CHROME"};

static const Theme T_MIAMI = {
    rgb(4, 2, 14),      // bg
    rgb(14, 6, 26),     // hdrBg
    rgb(255, 0, 128),   // accent1
    rgb(0, 240, 255),   // accent2n
    rgb(255, 215, 0),   // accent3w
    rgb(255, 225, 248), // textBrightp
    rgb(140, 115, 185), // textMide
    rgb(55, 35, 80),    // textDims
    rgb(16, 8, 32),     // barBg
    rgb(28, 8, 48),     // selRow
    "MIAMI VICE"};

static const Theme T_ASH = {
    rgb(16, 16, 16),    // bg
    rgb(22, 22, 22),    // hdrBg
    rgb(255, 255, 255), // accent1
    rgb(180, 180, 180), // accent2
    rgb(140, 140, 140), // accent3
    rgb(230, 230, 230), // textBright
    rgb(140, 140, 140), // textMid
    rgb(60, 60, 60),    // textDim
    rgb(35, 35, 35),    // barBg
    rgb(30, 30, 30),    // selRow
    "ASH"};

static const Theme T_LIGHT = {
    rgb(242, 242, 245), // bg
    rgb(255, 255, 255), // hdrBg
    rgb(250, 45, 85),   // accent1 (soft coral/pink)
    rgb(130, 140, 160), // accent2 (muted slate)
    rgb(255, 159, 110), // accent3 (soft peach)
    rgb(28, 28, 30),    // textBright (near-black)
    rgb(120, 120, 128), // textMid
    rgb(199, 199, 204), // textDim
    rgb(225, 225, 230), // barBg
    rgb(255, 232, 235), // selRow (light coral tint)
    "MODERN LIGHT"};

static const Theme *THEMES[6] = {&T_NEON, &T_TERM, &T_CORP, &T_MIAMI, &T_ASH, &T_LIGHT};