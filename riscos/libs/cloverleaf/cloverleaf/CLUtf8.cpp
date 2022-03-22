//
// Created by lenz on 5/24/20.
//

#include "CLUtf8.h"
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <oslib/osbyte.h>
#include <oslib/territory.h>
#include <oslib/serviceinternational.h>
#include <iconv.h>
#include <cloverleaf/CLException.h>
#include <cloverleaf/Logger.h>

#ifndef NOF_ELEMENTS
#define NOF_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))
#endif

/* Common values (ASCII) */
#define common								\
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,	\
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,	\
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,	\
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,	\
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,	\
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,	\
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,	\
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127	\

/* 0x8c->0x9F, used by many of the encodings */
#define common2								\
  0x2026, 0x2122, 0x2030, 0x2022, 0x2018, 0x2019, 0x2039, 0x203a,	\
  0x201c, 0x201d, 0x201e, 0x2013, 0x2014, 0x2212, 0x0152, 0x0153,	\
  0x2020, 0x2021, 0xfb01, 0xfb02

static const int latin1_table[256] =
        {
                common,
                0x20ac, 0x0174, 0x0175, -1, -1, 0x0176, 0x0177, -1, -1, -1, -1, -1,
                common2,
                160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
                176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
                192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
                208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
                224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
                240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
        };

static const int latin2_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
                0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
                0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
                0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
                0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
                0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
                0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
                0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
                0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
                0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
                0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
                0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
        };

static const int latin3_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, -1, 0x0124, 0x00A7,
                0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, -1, 0x017B,
                0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7,
                0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, -1, 0x017C,
                0x00C0, 0x00C1, 0x00C2, -1, 0x00C4, 0x010A, 0x0108, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                -1, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7,
                0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, -1, 0x00E4, 0x010B, 0x0109, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                -1, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,
                0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9
        };

static const int latin4_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7,
                0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF,
                0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7,
                0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B,
                0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
                0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A,
                0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
                0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF,
                0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
                0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B,
                0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
                0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9
        };

static const int latin5_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
                0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
                0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
                0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
                0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
                0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF
        };

static const int latin6_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7,
                0x013B, 0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A,
                0x00B0, 0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7,
                0x013C, 0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B,
                0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
                0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF,
                0x00D0, 0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168,
                0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
                0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
                0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF,
                0x00F0, 0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169,
                0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138
        };

static const int latin7_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0x2026, 0x2122, 0x2030, 0x2022, 0x2018, -1, 0x2039, 0x203a,
                -1, -1, -1, 0x2013, 0x2014, 0x2212, 0x0152, 0x0153,
                0x2020, 0x2021, 0xfb01, 0xfb02,
                0x00A0, 0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7,
                0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7,
                0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
                0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
                0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
                0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
                0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
                0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
                0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
                0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
                0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019
        };

static const int latin8_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7,
                0x1E80, 0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
                0x1E1E, 0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56,
                0x1E81, 0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61,
                0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x1E6A,
                0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x1E6B,
                0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF
        };

static const int latin9_table[256] =
        {
                common,
                -1, 0x0174, 0x0175, -1, -1, 0x0176, 0x0177, -1, -1, -1, -1, -1,
                0x2026, 0x2122, 0x2030, 0x2022, 0x2018, 0x2019, 0x2039, 0x203a,
                0x201c, 0x201d, 0x201e, 0x2013, 0x2014, 0x2212, -1, -1,
                0x2020, 0x2021, 0xfb01, 0xfb02,
                0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7,
                0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7,
                0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
                0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
                0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
                0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
        };

static const int latin10_table[256] =
        {
                common,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0x2026, 0x2122, 0x2030, 0x2022, 0x2018, 0x2019, 0x2039, 0x203a,
                0x201c, -1, -1, 0x2013, 0x2014, 0x2212, -1, -1,
                0x2020, 0x2021, 0xfb01, 0xfb02,
                0x00A0, 0x0104, 0x0105, 0x0141, 0x20AC, 0x201E, 0x0160, 0x00a7,
                0x0161, 0x00A9, 0x0218, 0x00AB, 0x0179, 0x00AD, 0x017A, 0x017B,
                0x00B0, 0x00B1, 0x010C, 0x0142, 0x017D, 0x201D, 0x00B6, 0x00B7,
                0x017E, 0x010D, 0x0219, 0x00BB, 0x0152, 0x0153, 0x0178, 0x017C,
                0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0106, 0x00C6, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                0x0110, 0x0143, 0x00D2, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x015A,
                0x0170, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0118, 0x021A, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x0107, 0x00E6, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                0x0111, 0x0144, 0x00F2, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x015B,
                0x0171, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0119, 0x021B, 0x00FF
        };

static const int welsh_table[256] =
        {
                common,
                0x20ac, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                common2,
                0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
                0x1E80, 0x00A9, 0x1E82, 0x00AB, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
                0x1E81, 0x00B9, 0x1E83, 0x00BB, 0x1EF3, 0x1E84, 0x1E85, 0x00BF,
                0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
                0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
                0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
                0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
                0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
                0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
                0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
                0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF
        };

static const int greek_table[256] =
        {
                common,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0x00A0, 0x2018, 0x2019, 0x00A3, 0x20AC, 0x20AF, 0x00A6, 0x00A7,
                0x00A8, 0x00A9, 0x037A, 0x00AB, 0x00AC, 0x00AD, 0x037E, 0x2015,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x0387,
                0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
                0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
                0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
                0x03A0, 0x03A1, -1, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
                0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
                0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
                0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
                0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
                0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, -1
        };

static const int cyrillic_table[256] =
        {
                common,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
                0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
                0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
                0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
                0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
                0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
                0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
                0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
                0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
                0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
                0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
                0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F
        };

static const int hebrew_table[256] =
        {
                common,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0x00A0, -1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
                0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x203E,
                0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
                0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, -1,
                -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, 0x2017,
                0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
                0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
                0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
                0x05E8, 0x05E9, 0x05EA, -1, -1, 0x200E, 0x200F, -1
        };

/**
 * Retrieve UCS table (above), given alphabet number
 *
 * \param alphabet  The RISC OS alphabet number
 * \return pointer to table, or NULL if not found
 */
const int *ucstable_from_alphabet(int alphabet)
{
    switch (alphabet) {
        case territory_ALPHABET_LATIN1:
            return latin1_table;
        case territory_ALPHABET_LATIN2:
            return latin2_table;
        case territory_ALPHABET_LATIN3:
            return latin3_table;
        case territory_ALPHABET_LATIN4:
            return latin4_table;
        case territory_ALPHABET_LATIN5:
            return latin5_table;
        case territory_ALPHABET_LATIN6:
            return latin6_table;
        case 114: /* Latin7 */
            return latin7_table;
        case 115: /* Latin8 */
            return latin8_table;
        case 116: /* Latin10 */
            return latin10_table;
        case territory_ALPHABET_LATIN9:
            return latin9_table;
        case territory_ALPHABET_WELSH:
            return welsh_table;
        case territory_ALPHABET_GREEK:
            return greek_table;
        case territory_ALPHABET_CYRILLIC:
            return cyrillic_table;
        case territory_ALPHABET_HEBREW:
            return hebrew_table;
        default:
            return nullptr;
    }

}


//static const char *localencodings[] = {
//        "ISO-8859-1//TRANSLIT",	/* BFont - 100 - just use Latin1, instead */
//        "ISO-8859-1//TRANSLIT",
//        "ISO-8859-2//TRANSLIT",
//        "ISO-8859-3//TRANSLIT",
//        "ISO-8859-4//TRANSLIT",
//        "ISO-8859-5//TRANSLIT",
//        "ISO-8859-6//TRANSLIT",
//        "ISO-8859-7//TRANSLIT",
//        "ISO-8859-8//TRANSLIT",
//        "ISO-8859-9//TRANSLIT",
//        "ISO-IR-182//TRANSLIT",
//        "UTF-8",
//        "ISO-8859-15//TRANSLIT",
//        "ISO-8859-10//TRANSLIT",
//        "ISO-8859-13//TRANSLIT",
//        "ISO-8859-14//TRANSLIT",
//        "ISO-8859-16//TRANSLIT",
//#define CONT_ENC_END 116	/* RISC OS alphabet numbers lie in a
//				 * contiguous range [100,CONT_ENC_END]
//				 * _except_ for Cyrillic2, which doesn't.
//				 */
//        "CP866//TRANSLIT"	/* Cyrillic2 - 120 */
//};
//
//static const struct special {
//    char local;		/**< Local 8bit representation */
//    char len;		/**< Length (in bytes) of UTF-8 character */
//    const char *utf;	/**< UTF-8 representation */
//} special_chars[] = {
//        { 0x80, 3, "\xE2\x82\xAC" },	/* EURO SIGN */
//        { 0x81, 2, "\xC5\xB4" },	/* LATIN CAPITAL LETTER W WITH CIRCUMFLEX */
//        { 0x82, 2, "\xC5\xB5" },	/* LATIN SMALL LETTER W WITH CIRCUMFLEX */
//        { 0x84, 3, "\xE2\x9C\x98" },	/* HEAVY BALLOT X */
//        { 0x85, 2, "\xC5\xB6" },	/* LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */
//        { 0x86, 2, "\xC5\xB7" },	/* LATIN SMALL LETTER Y WITH CIRCUMFLEX */
//        { 0x88, 3, "\xE2\x87\x90" },	/* LEFTWARDS DOUBLE ARROW */
//        { 0x89, 3, "\xE2\x87\x92" },	/* RIGHTWARDS DOUBLE ARROW */
//        { 0x8a, 3, "\xE2\x87\x93" },	/* DOWNWARDS DOUBLE ARROW */
//        { 0x8b, 3, "\xE2\x87\x91" },	/* UPWARDS DOUBLE ARROW */
//        { 0x8c, 3, "\xE2\x80\xA6" },	/* HORIZONTAL ELLIPSIS */
//        { 0x8d, 3, "\xE2\x84\xA2" },	/* TRADE MARK SIGN */
//        { 0x8e, 3, "\xE2\x80\xB0" },	/* PER MILLE SIGN */
//        { 0x8f, 3, "\xE2\x80\xA2" },	/* BULLET */
//        { 0x90, 3, "\xE2\x80\x98" },	/* LEFT SINGLE QUOTATION MARK */
//        { 0x91, 3, "\xE2\x80\x99" },	/* RIGHT SINGLE QUOTATION MARK */
//        { 0x92, 3, "\xE2\x80\xB9" },	/* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
//        { 0x93, 3, "\xE2\x80\xBA" },	/* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
//        { 0x94, 3, "\xE2\x80\x9C" },	/* LEFT DOUBLE QUOTATION MARK */
//        { 0x95, 3, "\xE2\x80\x9D" },	/* RIGHT DOUBLE QUOTATION MARK */
//        { 0x96, 3, "\xE2\x80\x9E" },	/* DOUBLE LOW-9 QUOTATION MARK */
//        { 0x97, 3, "\xE2\x80\x93" },	/* EN DASH */
//        { 0x98, 3, "\xE2\x80\x94" },	/* EM DASH */
//        { 0x99, 3, "\xE2\x88\x92" },	/* MINUS SIGN */
//        { 0x9a, 2, "\xC5\x92" },	/* LATIN CAPITAL LIGATURE OE */
//        { 0x9b, 2, "\xC5\x93" },	/* LATIN SMALL LIGATURE OE */
//        { 0x9c, 3, "\xE2\x80\xA0" },	/* DAGGER */
//        { 0x9d, 3, "\xE2\x80\xA1" },	/* DOUBLE DAGGER */
//        { 0x9e, 3, "\xEF\xAC\x81" },	/* LATIN SMALL LIGATURE FI */
//        { 0x9f, 3, "\xEF\xAC\x82" } 	/* LATIN SMALL LIGATURE FL */
//};
//
//// typedef unsigned long uint32_t;
//
///* Cache of previous iconv conversion descriptor used by utf8_convert */
//static struct {
//    char from[32];	/**< Encoding name to convert from */
//    char to[32];	/**< Encoding name to convert to */
//    iconv_t cd;	/**< Iconv conversion descriptor */
//} last_cd;
//
//static inline void utf8_clear_cd_cache(void)
//{
//    last_cd.from[0] = '\0';
//    last_cd.to[0] = '\0';
//    last_cd.cd = 0;
//}
//
///**
// * Convert a string from one encoding to another
// *
// * \param in_str  The NULL-terminated string to convert
// * \param len     Length of input string to consider (in bytes), or 0
// * \param from    The encoding name to convert from
// * \param to      The encoding name to convert to
// * \param result  Pointer to location in which to store result.
// * \param result_len Pointer to location in which to store result length.
// * \return NSERROR_OK for no error, NSERROR_NOMEM on allocation error,
// *         NSERROR_BAD_ENCODING for a bad character encoding
// */
//static void
//utf8_convert(const char *in_str,
//             size_t len,
//             const char *from,
//             const char *to,
//             char **result,
//             size_t *result_len)
//{
//    iconv_t cd;
//    char *temp, *out, *in;
//    size_t slen, rlen;
//
//    assert(in_str && from && to && result);
//
//    if (in_str[0] == '\0') {
//        /* On AmigaOS, iconv() returns an error if we pass an
//         * empty in_str.  This prevents iconv() being called as
//         * there is no conversion necessary anyway. */
//        *result = strdup("");
//        if (!(*result)) {
//            *result = NULL;
//            throw_bad_alloc_exception();
//        }
//        return;
//    }
//
//    if (strcasecmp(from, to) == 0) {
//        /* conversion from an encoding to itself == strdup */
//        slen = len ? len : strlen(in_str);
//        *(result) = strndup(in_str, slen);
//        if (!(*result)) {
//            *(result) = NULL;
//            throw_bad_alloc_exception();
//        }
//
//        return;
//    }
//
//    in = (char *)in_str;
//
//    /* we cache the last used conversion descriptor,
//     * so check if we're trying to use it here */
//    if (strncasecmp(last_cd.from, from, sizeof(last_cd.from)) == 0 &&
//        strncasecmp(last_cd.to, to, sizeof(last_cd.to)) == 0) {
//        cd = last_cd.cd;
//    }
//    else {
//        /* no match, so create a new cd */
//        cd = iconv_open(to, from);
//        if (cd == (iconv_t)-1) {
//            if (errno == 22)
//                throw_exception("bad_encoding");
//            /* default to no memory */
//            throw_bad_alloc_exception();
//        }
//
//        /* close the last cd - we don't care if this fails */
//        if (last_cd.cd)
//            iconv_close(last_cd.cd);
//
//        /* and copy the to/from/cd data into last_cd */
//        snprintf(last_cd.from, sizeof(last_cd.from), "%s", from);
//        snprintf(last_cd.to, sizeof(last_cd.to), "%s", to);
//        last_cd.cd = cd;
//    }
//
//    slen = len ? len : strlen(in_str);
//    /* Worst case = ASCII -> UCS4, so allocate an output buffer
//     * 4 times larger than the input buffer, and add 4 bytes at
//     * the end for the NULL terminator
//     */
//    rlen = slen * 4 + 4;
//
//    temp = out = (char*) malloc(rlen);
//    if (!out) {
//        throw_bad_alloc_exception();
//    }
//
//    /* perform conversion */
//    if (iconv(cd, &in, &slen, &out, &rlen) == (size_t)-1) {
//        free(temp);
//        /* clear the cached conversion descriptor as it's invalid */
//        if (last_cd.cd)
//            iconv_close(last_cd.cd);
//        utf8_clear_cd_cache();
//        /** \todo handle the various cases properly
//         * There are 3 possible error cases:
//         * a) Insufficiently large output buffer
//         * b) Invalid input byte sequence
//         * c) Incomplete input sequence */
//        throw_bad_alloc_exception();
//    }
//
//    *(result) = (char*)realloc(temp, out - temp + 4);
//    if (!(*result)) {
//        free(temp);
//        *(result) = NULL; /* for sanity's sake */
//        throw_bad_alloc_exception();
//    }
//
//    /* NULL terminate - needs 4 characters as we may have
//     * converted to UTF-32 */
//    memset((*result) + (out - temp), 0, 4);
//
//    if (result_len != NULL) {
//        *result_len = (out - temp);
//    }
//}
//
//void utf8_to_enc(const char *in_str, const char *encname,
//                 size_t len, char **result)
//{
//    utf8_convert(in_str, len, "UTF-8", encname, result, NULL);
//}
//
//void utf8_from_enc(const char *in_str, const char *encname,
//                   size_t len, char **result, size_t *result_len)
//{
//    utf8_convert(in_str, len, encname, "UTF-8", result, result_len);
//}

/**
 * Find next legal UTF-8 char in string
 *
 * \param s        The string (assumed valid)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \param error    Location to receive error code
 */
#define UTF8_NEXT(s, len, off, nextoff, error)                          \
do {                                                                    \
        if (s == NULL || off >= len || nextoff == NULL) {               \
                error = 1;                            \
                break;                                                  \
        }                                                               \
                                                                        \
        /* Skip current start byte (if present - may be mid-sequence) */\
        if (s[off] < 0x80 || (s[off] & 0xC0) == 0xC0)                   \
                off++;                                                  \
                                                                        \
        while (off < len && (s[off] & 0xC0) == 0x80)                    \
                off++;                                                  \
                                                                        \
        *nextoff = off;                                                 \
                                                                        \
        error = 0;                                         \
} while(0)

size_t utf8_next(const char *s, size_t l, size_t o)
{
    uint32_t next;
    int perror;

    UTF8_NEXT(s, l, o, &next, perror);
    assert(perror == 0);

    return next;
}


/**
 * Convert a UTF-8 encoded string into the system local encoding
 *
 * \param string The string to convert
 * \param len The length (in bytes) of the string, or 0
 * \param result Pointer to location in which to store result
 */
//void utf8_to_local_encoding(const char *in_str, char **result)
//{
//    os_error *error;
//    int alphabet, i;
//    size_t off, prev_off, len;
//    char *temp, *cur_pos;
//    const char *enc;
//
//    assert(in_str);
//    assert(result);
//
//    /* get length, if necessary */
//    len = strlen(in_str);
//
//    /* read system alphabet */
//    error = xosbyte1(osbyte_ALPHABET_NUMBER, 127, 0, &alphabet);
//    if (error)
//        alphabet = territory_ALPHABET_LATIN1;
//
//    /* UTF-8 -> simply copy in_str */
//    if (alphabet == 111 /* UTF-8 */) {
//        *result = strndup(in_str, len);
//        return;
//    }
//
//    /* get encoding name */
//    enc = (alphabet <= CONT_ENC_END ? localencodings[alphabet - 100]
//                                    : (alphabet == 120 ?
//                                       localencodings[CONT_ENC_END - 100 + 1]
//                                                       : localencodings[0]));
//
//    /* create output buffer */
//    *(result) = (char*) malloc(len + 1);
//    if (!(*result))
//        throw_bad_alloc_exception();
//    *(*result) = '\0';
//
//    prev_off = 0;
//    cur_pos = (*result);
//
//    /* Iterate over string, converting input between unconvertable
//     * characters and inserting appropriate output for characters
//     * that iconv can't handle. */
//    for (off = 0; off < len; off = utf8_next(in_str, len, off)) {
//        if (in_str[off] != 0xE2 &&
//            in_str[off] != 0xC5 && in_str[off] != 0xEF)
//            continue;
//
//        for (i = 0; i != NOF_ELEMENTS(special_chars); i++) {
//            if (strncmp(in_str + off, special_chars[i].utf,
//                        special_chars[i].len) != 0)
//                continue;
//
//            /* 0 length has a special meaning to utf8_to_enc */
//            if (off - prev_off > 0) {
//                try {
//
//                    utf8_to_enc(in_str + prev_off, enc,
//                                off - prev_off, &temp);
//                } catch(const CLException& e) {
//                    free(*result);
//                    throw_bad_alloc_exception();
//                }
//
//                strcat(cur_pos, temp);
//
//                cur_pos += strlen(temp);
//
//                free(temp);
//            }
//
//            *cur_pos = special_chars[i].local;
//            *(++cur_pos) = '\0';
//            prev_off = off + special_chars[i].len;
//        }
//    }
//
//    /* handle last chunk
//     * NB. 0 length has a special meaning to utf8_to_enc */
//
//    if (prev_off < len) {
//        try {
//            utf8_to_enc(in_str + prev_off, enc, len - prev_off,
//                        &temp);
//        } catch(const CLException& e) {
//            free(*result);
//            throw_bad_alloc_exception();
//        }
//
//        strcat(cur_pos, temp);
//
//        free(temp);
//    }
//    Logger::debug("[%s] utf8->local [%s]", in_str, *result);
//}

/**
 * Convert a string encoded in the system local encoding to UTF-8
 *
 * \param in_str The string to convert
 * \param len The length (in bytes) of the string, or 0
 * \param result Pointer to location in which to store result
 */
void get_riscos_ucs_table(int *alphabet, const int **ucs_ostable) {
    os_error *error;
    osbool unclaimed;

    error = xosbyte1(osbyte_ALPHABET_NUMBER, 127, 0, alphabet);
    if (error) {
        *alphabet = territory_ALPHABET_LATIN1;
    }

    if (*alphabet == territory_ALPHABET_UTF8) {
        *ucs_ostable = nullptr;
        return;
    }
    error = xserviceinternational_get_ucs_conversion_table(*alphabet, &unclaimed, (void **)ucs_ostable);
//    Logger::error("alphabeth: %d error:%x", *alphabet, error);
    if (error != NULL) {
        Logger::error("failed reading UCS conversion table: 0x%x: %s", error->errnum, error->errmess);
        /* Try using our own table instead */
        *ucs_ostable = ucstable_from_alphabet(*alphabet);
    } else if (unclaimed) {
        /* Service wasn't claimed so use our own ucstable */
        Logger::error("serviceinternational_get_ucs_conversion_table unclaimed, using internal conversion table");
        *ucs_ostable = ucstable_from_alphabet(*alphabet);
    }
}

void local_to_utf8_encoding(const char *src, char **dst) {
    int i, j, alphabet, dst_size, src_size = strlen(src);
    char *res;
    const int *ucs_ostable;
    u_int32_t *ucs_str;

    get_riscos_ucs_table(&alphabet, &ucs_ostable);

    if (alphabet == territory_ALPHABET_UTF8) {
        res = strdup(src);
        *dst = res;
        return;
    }
    ucs_str = (u_int32_t *)malloc(src_size*4+4);
    if (!ucs_str) {
        *dst = NULL;
        throw_bad_alloc_exception();
    }
    for(i = 0, j = 0; i < src_size; i++) {
        if (ucs_ostable[src[i]] != 0xffffffff) {
            ucs_str[j++] = ucs_ostable[src[i]];
        }
    }
    dst_size = j*4+4;
    res = (char*)malloc(dst_size);
    if (!res) {
        free(ucs_str);
        *dst = NULL;
        throw_bad_alloc_exception();
    }
    ucs4_to_utf8(res, dst_size, ucs_str, j);
    free(ucs_str);
    *dst = res;
    Logger::debug("[%s] local->utf8 [%s]", src, res);
}

void utf8_to_local_encoding(const char *src, char **dst) {
    int i, j, alphabet, ucs_size, src_size = strlen(src), result_ptr = 0;
    char *res;
    const int *ucs_ostable;
    u_int32_t *ucs_str;

    get_riscos_ucs_table(&alphabet, &ucs_ostable);
//    {
//        for (int i = 0; i < 16; i++) {
//            for (int j = 0; j < 16; j++) {
//                Logger::debug("tbl %04x ", ucs_ostable[j + i * 16]);
//            }
//        }
//    }

    if (alphabet == territory_ALPHABET_UTF8) {
        res = strdup(src);
        *dst = res;
        return;
    }
    ucs_size = src_size*4+4;
    ucs_str = (u_int32_t *)malloc(ucs_size);
    if (!ucs_str) {
        *dst = NULL;
        throw_bad_alloc_exception();
    }

    utf8_to_ucs4(ucs_str, ucs_size, src, -1);

    res = (char*)malloc(src_size+1);
    if (!res) {
        free(ucs_str);
        *dst = NULL;
        throw_bad_alloc_exception();
    }

    for(i = 0; ucs_str[i] != 0; i++) {
//        Logger::debug("%08x ", ucs_str[i]);
        for(j = 0; j < 256; j++) {
            if (ucs_str[i] == ucs_ostable[j]) {
                res[result_ptr] = (unsigned char)j;
//                Logger::debug("%08x -> %c", res[result_ptr], (unsigned char)j);
                result_ptr++;
                break;
            }
        }
    }
    res[result_ptr] = 0;
    free(ucs_str);
    *dst = res;
    Logger::debug("[%s] utf8->local [%s]", src, res);
}


std::string riscos_local_to_utf8(const std::string &in_str) {
    char* utf8txt;
    local_to_utf8_encoding(in_str.c_str(), &utf8txt);
    std::string result = std::string(utf8txt);
    free(utf8txt);
    return result;
}

std::string utf8_to_riscos_local(const std::string &in_utf8_str) {
    char* local_str;
    utf8_to_local_encoding(in_utf8_str.c_str(), &local_str);
    std::string result = std::string(local_str);
    free(local_str);
    return result;
}

unsigned int utf8_len_bytes_substr(const std::string& str, unsigned int start, unsigned int leng) {
    if (leng==0) { return 0; }
    unsigned int c, i, ix, q, min=std::string::npos, max=std::string::npos;
    for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
    {
        if (q==start){ min=i; }
        if (q<=start+leng || leng==std::string::npos){ max=i; }

        c = (unsigned char) str[i];
        if      (c>=0   && c<=127) i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
            //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
            //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return 0;//invalid utf8
    }
    if (q<=start+leng || leng==std::string::npos){ max=i; }
    if (min==std::string::npos || max==std::string::npos) { return 0; }
    return max - min;
}


std::string utf8_substr(const std::string& str, unsigned int start, unsigned int leng) {
    if (leng==0) { return ""; }
    unsigned int c, i, ix, q, min=std::string::npos, max=std::string::npos;
    for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
    {
        if (q==start){ min=i; }
        if (q<=start+leng || leng==std::string::npos){ max=i; }

        c = (unsigned char) str[i];
        if      (c>=0   && c<=127) i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
            //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
            //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return "";//invalid utf8
    }
    if (q<=start+leng || leng==std::string::npos){ max=i; }
    if (min==std::string::npos || max==std::string::npos) { return ""; }
//    Logger::debug("str.substr(min,max) start:%d len:%d min:%d max:%d %s->%s", start, leng, min, max, str.c_str(), str.substr(min,max - min).c_str());
    return str.substr(min,max - min);
}

// support negative indexes
std::string utf8_substr2(const std::string &str,int start, int length) {
    int i,ix,j,realstart,reallength;
    if (length==0) return "";
    if (start<0 || length <0)
    {
        //find j=utf8_strlen(str);
        for(j=0,i=0,ix=str.length(); i<ix; i+=1, j++)
        {
            unsigned char c= str[i];
            if      (c>=0   && c<=127) i+=0;
            else if (c>=192 && c<=223) i+=1;
            else if (c>=224 && c<=239) i+=2;
            else if (c>=240 && c<=247) i+=3;
            else if (c>=248 && c<=255) return "";//invalid utf8
        }
        if (length !=INT_MAX && j+length-start<=0) return "";
        if (start  < 0 ) start+=j;
        if (length < 0 ) length=j+length-start;
    }

    j=0,realstart=0,reallength=0;
    for(i=0,ix=str.length(); i<ix; i+=1, j++)
    {
        if (j==start) { realstart=i; }
        if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
        unsigned char c= str[i];
        if      (c>=0   && c<=127) i+=0;
        else if (c>=192 && c<=223) i+=1;
        else if (c>=224 && c<=239) i+=2;
        else if (c>=240 && c<=247) i+=3;
        else if (c>=248 && c<=255) return "";//invalid utf8
    }
    if (j==start) { realstart=i; }
    if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }

    return str.substr(realstart,reallength);
}


static const u_int32_t offsetsFromUTF8[6] = {
        0x00000000UL, 0x00003080UL, 0x000E2080UL,
        0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const char trailingBytesForUTF8[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/* conversions without error checking
   only works for valid UTF-8, i.e. no 5- or 6-byte sequences
   srcsz = source size in bytes, or -1 if 0-terminated
   sz = dest size in # of wide characters

   returns # characters converted
   dest will always be L'\0'-terminated, even if there isn't enough room
   for all the characters.
   if sz = srcsz+1 (i.e. 4*srcsz+4 bytes), there will always be enough space.
*/
int utf8_to_ucs4(u_int32_t *dest, int sz, const char *src, int srcsz) {
    u_int32_t ch;
    char *src_end = const_cast<char *>(src + srcsz);
    int nb;
    int i=0;

    while (i < sz-1) {
        nb = trailingBytesForUTF8[(unsigned char)*src];
        if (srcsz == -1) {
            if (*src == 0)
                goto done_toucs;
        } else {
            if (src + nb >= src_end)
                goto done_toucs;
        }
        ch = 0;
        switch (nb) {
            /* these fall through deliberately */
            case 3: ch += (unsigned char)*src++; ch <<= 6;
            case 2: ch += (unsigned char)*src++; ch <<= 6;
            case 1: ch += (unsigned char)*src++; ch <<= 6;
            case 0: ch += (unsigned char)*src++;
        }
        ch -= offsetsFromUTF8[nb];
        dest[i++] = ch;
    }
    done_toucs:
    dest[i] = 0;
    return i;
}

/* srcsz = number of source characters, or -1 if 0-terminated
   sz = size of dest buffer in bytes

   returns # characters converted
   dest will only be '\0'-terminated if there is enough space. this is
   for consistency; imagine there are 2 bytes of space left, but the next
   character requires 3 bytes. in this case we could NUL-terminate, but in
   general we can't when there's insufficient space. therefore this function
   only NUL-terminates if all the characters fit, and there's space for
   the NUL as well.
   the destination string will never be bigger than the source string.
*/
int ucs4_to_utf8(char *dest, int sz, u_int32_t *src, int srcsz) {
    u_int32_t ch;
    int i = 0;
    char *dest_end = dest + sz;

    while (srcsz<0 ? src[i]!=0 : i < srcsz) {
        ch = src[i];
        if (ch < 0x80) {
            if (dest >= dest_end)
                return i;
            *dest++ = (char)ch;
        } else if (ch < 0x800) {
            if (dest >= dest_end-1)
                return i;
            *dest++ = (ch>>6) | 0xC0;
            *dest++ = (ch & 0x3F) | 0x80;
        } else if (ch < 0x10000) {
            if (dest >= dest_end-2)
                return i;
            *dest++ = (ch>>12) | 0xE0;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        } else if (ch < 0x110000) {
            if (dest >= dest_end-3)
                return i;
            *dest++ = (ch>>18) | 0xF0;
            *dest++ = ((ch>>12) & 0x3F) | 0x80;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        i++;
    }
    if (dest < dest_end)
        *dest = '\0';
    return i;
}

int ucs4_wc_to_utf8(char *dest, u_int32_t ch) {
    if (ch < 0x80) {
        dest[0] = (char)ch;
        return 1;
    }
    if (ch < 0x800) {
        dest[0] = (ch>>6) | 0xC0;
        dest[1] = (ch & 0x3F) | 0x80;
        return 2;
    }
    if (ch < 0x10000) {
        dest[0] = (ch>>12) | 0xE0;
        dest[1] = ((ch>>6) & 0x3F) | 0x80;
        dest[2] = (ch & 0x3F) | 0x80;
        return 3;
    }
    if (ch < 0x110000) {
        dest[0] = (ch>>18) | 0xF0;
        dest[1] = ((ch>>12) & 0x3F) | 0x80;
        dest[2] = ((ch>>6) & 0x3F) | 0x80;
        dest[3] = (ch & 0x3F) | 0x80;
        return 4;
    }
    return 0;
}
