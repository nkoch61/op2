/* keytable.c */


#include <stddef.h>

#include "common.h"
#include "keytable.h"


const __flash KeyTableEntry keytable[ROWS*COLUMNS] =
{
  [0x5c] = {                               .m = 0x05,             .d = FSTR("F1"    ) },
  [0x64] = {                               .m = 0x06,             .d = FSTR("F2"    ) },
  [0x6c] = {                               .m = 0x04,             .d = FSTR("F3"    ) },
  [0x5d] = {                               .m = 0x0c,             .d = FSTR("F4"    ) },
  [0x65] = {                               .m = 0x03,             .d = FSTR("F5"    ) },
  [0x6d] = {                               .m = 0x0b,             .d = FSTR("F6"    ) },
  [0x5a] = {                               .m = 0x83,             .d = FSTR("F7"    ) },
  [0x62] = {                               .m = 0x0a,             .d = FSTR("F8"    ) },
  [0x6a] = {                               .m = 0x01,             .d = FSTR("F9"    ) },
  [0x58] = {                               .m = 0x09,             .d = FSTR("F10"   ) },
  [0x60] = {                               .m = 0x78,             .d = FSTR("F11"   ) },
  [0x68] = {                               .m = 0x07,             .d = FSTR("F12"   ) },
  [0x5f] = {                               .m = 0x08,             .d = FSTR("F13"   ) },
  [0x67] = {                               .m = 0x10,             .d = FSTR("F14"   ) },
  [0x6f] = {                               .m = 0x18,             .d = FSTR("F15"   ) },
  [0x74] = {                               .m = 0x76,             .d = FSTR("ESC"   ) },
  [0x76] = {                               .m = 0x0d,             .d = FSTR("TAB"   ) },
  [0x1a] = { .f = SPECIAL,                 .m = SPECIAL_CTRL,     .d = FSTR("CTRL"  ) },
  [0x18] = { .f = SPECIAL,                 .m = SPECIAL_SHIFT,    .d = FSTR("LSHIFT") },
  [0x19] = { .f = SPECIAL,                 .m = SPECIAL_SHIFT,    .d = FSTR("RSHIFT") },
  [0x1e] = { .f = SPECIAL,                 .m = SPECIAL_REP,      .d = FSTR("REP"   ) },
  [0x77] = { .f = E0_PREFIX,               .m = 0x75,             .d = FSTR("UP"    ) },
  [0x7f] = { .f = E0_PREFIX,               .m = 0x6b,             .d = FSTR("LEFT"  ) },
  [0x47] = {                               .m = 0x29,             .d = FSTR("SPACE" ) },
  [0x37] = { .f = E0_PREFIX,               .m = 0x74,             .d = FSTR("RIGHT" ) },
  [0x2f] = { .f = E0_PREFIX,               .m = 0x72,             .d = FSTR("DOWN"  ) },
  [0x1f] = {                               .m = SPECIAL_LOCK,     .d = FSTR("LOCK"  ) },
  [0x71] = {                               .m = 0x1c,             .d = FSTR("A"     ) },
  [0x48] = {                               .m = 0x32,             .d = FSTR("B"     ) },
  [0x50] = {                               .m = 0x21,             .d = FSTR("C"     ) },
  [0x79] = {                               .m = 0x23,             .d = FSTR("D"     ) },
  [0x7d] = {                               .m = 0x24,             .d = FSTR("E"     ) },
  [0x52] = {                               .m = 0x2b,             .d = FSTR("F"     ) },
  [0x51] = {                               .m = 0x34,             .d = FSTR("G"     ) },
  [0x4a] = {                               .m = 0x33,             .d = FSTR("H"     ) },
  [0x3e] = {                               .m = 0x43,             .d = FSTR("I"     ) },
  [0x41] = {                               .m = 0x3b,             .d = FSTR("J"     ) },
  [0x3a] = {                               .m = 0x42,             .d = FSTR("K"     ) },
  [0x31] = {                               .m = 0x4b,             .d = FSTR("L"     ) },
  [0x39] = {                               .m = 0x3a,             .d = FSTR("M"     ) },
  [0x40] = {                               .m = 0x31,             .d = FSTR("N"     ) },
  [0x35] = {                               .m = 0x44,             .d = FSTR("O"     ) },
  [0x36] = {                               .m = 0x4d,             .d = FSTR("P"     ) },
  [0x75] = {                               .m = 0x15,             .d = FSTR("Q"     ) },
  [0x56] = {                               .m = 0x2d,             .d = FSTR("R"     ) },
  [0x7a] = {                               .m = 0x1b,             .d = FSTR("S"     ) },
  [0x55] = {                               .m = 0x2c,             .d = FSTR("T"     ) },
  [0x45] = {                               .m = 0x3c,             .d = FSTR("U"     ) },
  [0x49] = {                               .m = 0x2a,             .d = FSTR("V"     ) },
  [0x7e] = {                               .m = 0x1d,             .d = FSTR("W"     ) },
  [0x78] = {                               .m = 0x22,             .d = FSTR("X"     ) },
  [0x70] = {                               .m = 0x1a,             .d = FSTR("Y"     ) },
  [0x4e] = {                               .m = 0x35,             .d = FSTR("Z"     ) },
  [0x73] = {                               .m = 0x16,             .d = FSTR("1 !"   ) },
  [0x7c] = {                               .m = 0x1e,             .d = FSTR("2 \""  ) },
  [0x7b] = { .f = SPECIAL,                 .m = SPECIAL_3,        .d = FSTR("3 #"   ) },
  [0x54] = {                               .m = 0x25,             .d = FSTR("4 $"   ) },
  [0x53] = {                               .m = 0x2e,             .d = FSTR("5 %"   ) },
  [0x4c] = {                               .m = 0x36,             .d = FSTR("6 &"   ) },
  [0x43] = { .f = SPECIAL,                 .m = SPECIAL_7,        .d = FSTR("7 '"   ) },
  [0x3c] = {                               .m = 0x3e,             .d = FSTR("8 ("   ) },
  [0x33] = {                               .m = 0x46,             .d = FSTR("9 )"   ) },
  [0x34] = { .f = SPECIAL,                 .m = SPECIAL_0,        .d = FSTR("0 @"   ) },
  [0x2b] = { .f = SPECIAL,                 .m = SPECIAL_SZ,       .d = FSTR("sz ~ ^") },
  [0x2c] = { .f = SPECIAL,                 .m = SPECIAL_UL,       .d = FSTR("_ ="   ) },
  [0x23] = { .f = SPECIAL,                 .m = SPECIAL_DEL,      .d = FSTR("DEL"   ) },
  [0x1c] = { .f = SPECIAL,                 .m = SPECIAL_RES,      .d = FSTR("RES"   ) },
  [0x38] = { .f = SPECIAL,                 .m = SPECIAL_COMMA,    .d = FSTR(", <"   ) },
  [0x30] = { .f = SPECIAL,                 .m = SPECIAL_PERIOD,   .d = FSTR(". >"   ) },
  [0x28] = { .f = SPECIAL,                 .m = SPECIAL_SLASH,    .d = FSTR("/ ?"   ) },
  [0x22] = { .f = SPECIAL,                 .m = SPECIAL_COLON,    .d = FSTR(": *"   ) },
  [0x2e] = { .f = SPECIAL,                 .m = SPECIAL_PLUS,     .d = FSTR("+ ;"   ) },
  [0x29] = { .f = SPECIAL,                 .m = SPECIAL_AE,       .d = FSTR("AE { [") },
  [0x32] = { .f = SPECIAL,                 .m = SPECIAL_OE,       .d = FSTR("OE \\" ) },
  [0x2d] = { .f = SPECIAL,                 .m = SPECIAL_UE,       .d = FSTR("UE } ]") },
  [0x13] = { .f = SPECIAL,                 .m = SPECIAL_PROG,     .d = FSTR("PROG"  ) },
  [0x14] = { .f = SPECIAL,                 .m = SPECIAL_NUMSTAR,  .d = FSTR("*"     ) },
  [0x0b] = { .f = SPECIAL,                 .m = SPECIAL_NUMMINUS, .d = FSTR("-"     ) },
  [0x0c] = { .f = SPECIAL,                 .m = SPECIAL_NUMPLUS,  .d = FSTR("+"     ) },
  [0x0d] = { .f = SPECIAL,                 .m = SPECIAL_NUMSLASH, .d = FSTR("./."   ) },
  [0x0f] = {                               .m = 0x71,             .d = FSTR("."     ) },
  [0x07] = { .f = SPECIAL,                 .m = SPECIAL_00,       .d = FSTR("00"    ) },
  [0x26] = {                               .m = 0x5a,             .d = FSTR("ENTER" ) },
  [0x0a] = { .f = E0_PREFIX,               .m = 0x5a,             .d = FSTR("enter" ) },
  [0x20] = { .f = SPECIAL,                 .m = SPECIAL_LF,       .d = FSTR("LF"    ) },
  [0x17] = {                               .m = 0x70,             .d = FSTR("0"     ) },
  [0x10] = {                               .m = 0x69,             .d = FSTR("1"     ) },
  [0x08] = {                               .m = 0x72,             .d = FSTR("2"     ) },
  [0x00] = {                               .m = 0x7a,             .d = FSTR("3"     ) },
  [0x12] = {                               .m = 0x6b,             .d = FSTR("4"     ) },
  [0x09] = {                               .m = 0x73,             .d = FSTR("5"     ) },
  [0x02] = {                               .m = 0x74,             .d = FSTR("6"     ) },
  [0x16] = {                               .m = 0x6c,             .d = FSTR("7"     ) },
  [0x15] = {                               .m = 0x75,             .d = FSTR("8"     ) },
  [0x0e] = {                               .m = 0x7d,             .d = FSTR("9"     ) },
};
