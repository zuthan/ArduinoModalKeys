#if !defined(__KEYMAP_H_)
#define __KEYMAP_H_

// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-14.html

// Custom Flags
#define _CustomModifier   (1 << 0)

// Modifier Flags
#define LCtrl   (1 << 0)
#define LShift  (1 << 1)
#define LAlt    (1 << 2)
#define LGui    (1 << 3)
#define RCtrl   (1 << 4)
#define RShift  (1 << 5)
#define RAlt    (1 << 6)
#define RGui    (1 << 7)

// Key Scan Codes
#define _A 4
#define _B 5
#define _C 6
#define _D 7
#define _E 8
#define _F 9
#define _G 10
#define _H 11
#define _I 12
#define _J 13
#define _K 14
#define _L 15
#define _M 16
#define _N 17
#define _O 18
#define _P 19
#define _Q 20
#define _R 21
#define _S 22
#define _T 23
#define _U 24
#define _V 25
#define _W 26
#define _X 27
#define _Y 28
#define _Z 29
#define _1 30
#define _2 31
#define _3 32
#define _4 33
#define _5 34
#define _6 35
#define _7 36
#define _8 37
#define _9 38
#define _0 39
#define _Enter 40
#define _Escape 41
#define _Backspace 42
#define _Tab 43
#define _Space 44
#define _Dash 45
#define _Equals 46
#define _LeftBracket 47
#define _RightBracket 48
#define _Backslash 49
#define _International2 50
#define _Semicolon 51
#define _Apostrophe 52
#define _Backtick 53
#define _Comma 54
#define _Fullstop 55
#define _ForwardSlash 56
#define _CapsLock 57
#define _F1 58
#define _F2 59
#define _F3 60
#define _F4 61
#define _F5 62
#define _F6 63
#define _F7 64
#define _F8 65
#define _F9 66
#define _F10 67
#define _F11 68
#define _F12 69
#define _PrintScreen 70
#define _ScrollLock 71
#define _Pause 72
#define _Insert 73
#define _Home 74
#define _PgUp 75
#define _Delete 76
#define _End 77
#define _PgDn 78
#define _Right 79
#define _Left 80
#define _Down 81
#define _Up 82
#define _NumLock 83
#define _NumpadDivide 84
#define _NumpadTimes 85
#define _NumpadMinus 86
#define _NumpadPlus 87
#define _NumpadEnter 88
#define _Numpad1 89
#define _Numpad2 90
#define _Numpad3 91
#define _Numpad4 92
#define _Numpad5 93
#define _Numpad6 94
#define _Numpad7 95
#define _Numpad8 96
#define _Numpad9 97
#define _Numpad0 98
#define _NumpadDot 99
#define _International1 100
#define _Menu 101

#define _F13 104
#define _F14 105
#define _F15 106
#define _F16 107
#define _F17 108
#define _F18 109
#define _F19 110
#define _F20 111
#define _F21 112
#define _F22 113
#define _F23 114
#define _F24 115

#define _Help 117

#define _Undo 122
#define _Cut 123
#define _Copy 124
#define _Paste 125

#define _Mute 127
#define _VolumeUp 128
#define _VolumeDown 129

#include <avr/pgmspace.h>
#include <Arduino.h>

// Mode definitions
struct RichKey {
    uint8_t mods;
    uint8_t key;
    uint8_t flags;
};

// typedef for functions that map keys to RichKeys
typedef RichKey(*KeyMap)(uint8_t *buf, uint8_t i);

// *******************************************************************************************
// modal_keys.ino functions
// *******************************************************************************************
bool operator==(const RichKey& lhs, const RichKey& rhs);
extern void Log(String text);
extern void PressKey(RichKey key);
extern void PressAndReleaseKey(RichKey key);

// *******************************************************************************************
// keymap.cpp functions
// *******************************************************************************************
extern void InitializeState();
extern void OnKeyboardEvent(uint8_t *prev_buf, uint8_t *cur_buf);
extern RichKey MapKey(uint8_t *buf, uint8_t i);
extern String GetStateString();

#endif // __KEYMAP_H_
