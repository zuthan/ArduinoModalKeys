#include "keymap.h"
#include <EEPROM.h>

// Map of where in EEPROM storage to store each config variable
#define OSModeSlot 0
#define KeyboardLayoutSlot 1

// ****************************************************************************
// Type Declarations
// ****************************************************************************

// Operating System Modes
typedef enum {
    Windows = 0,
    OSX
} OSMode;

// Keyboard Layouts
typedef enum {
    qwerty = 0,
    dvorak,
    colemak
} KeyboardLayout;

// the available keyboard modes
typedef enum
{
    NoKeys = 0,
    Configuration,
    Normal,
    RemnantKeys,
    LeftAlt,
    LeftMod,
    RightAlt,
    RightMod,
    AltTab,
    WindowSnap,
    NumPad,
    GamingNoKeys,
    GamingBacktick,
    GamingTab,
    GamingCapsLock,
    GamingSpace
} Mode;

typedef enum {
    Left = 0,
    Right
} Side;

typedef enum {
    Clean = 0,
    Used
} ModeState;

// ****************************************************************************
// Function Declarations
// ****************************************************************************

// helpers
void LoadOSMode();
void SetOSMode(OSMode osMode);
void SetMode(Mode mode, ModeState modeState);
ControlCode EnterMode(Mode mode, ModeState modeState);
ControlCode SendKey(uint8_t keycode, uint8_t outbuf[8]);
ControlCode SendModifiers(uint8_t mods, uint8_t outbuf[8]);
ControlCode SendKeyCombo(uint8_t mods, uint8_t keycode, uint8_t outbuf[8]);
ControlCode InvalidKey();
ControlCode MapKey(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
uint8_t NumKeysPressed(uint8_t buf[8]);
uint8_t NumModsPressed(uint8_t buf[8]);
uint8_t NumKeysOrModsPressed(uint8_t buf[8]);
String GetOSModeString(OSMode osMode);
String GetModeString(Mode mode);
String GetModeStateString(ModeState modeState);
String GetLayoutString(KeyboardLayout layout);

// state changing methods
ControlCode Configuration_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);

ControlCode DoNothing_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode Normal_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode EntryPoint_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode LeftAltMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode LeftModMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode RightAltMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode RightModMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode AltTab_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode WindowSnap_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode NumPad_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);

ControlCode GamingEntryPoint_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode GamingBacktick_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode GamingTab_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode GamingCapsLock_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);
ControlCode GamingSpace_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]);

// state handling callbacks
void HandleLastKeyReleased();

// ****************************************************************************
// Constants
// ****************************************************************************

const RichKey NoKey = { 0, 0, 0 };
const RichKey CustomModifierKey = { 0, 0, _CustomModifier };

// Keymap based on the scancodes from 4 to 57, refer to the HID usage table on the meaning of each element
const uint8_t qwertyKeymap[]  = {4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
const uint8_t dvorakKeymap[]  = {4, 27, 13,  8, 55, 24, 12,  7,  6, 11, 23, 17, 16,  5, 21, 15, 52, 19, 18, 28, 10, 14, 54, 20,  9, 51, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 56, 46, 49, 50, 22, 45, 53, 26, 25, 29, 57};
const uint8_t colemakKeymap[] = {4,  5,  6, 22,  9, 23,  7, 11, 24, 17,  8, 12, 16, 14, 28, 51, 20, 19, 21, 10, 15, 25, 26, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 18, 52, 53, 54, 55, 56, 42};

const uint8_t *Keymap[] =
{
    qwertyKeymap,
    dvorakKeymap,
    colemakKeymap
};

// array of KeyMaps, one for each mode
const KeyMap KeyMaps[] = {
    &EntryPoint_keymap,         /* NoKeys */
    &Configuration_keymap,      /* Configuration */
    &Normal_keymap,             /* Normal */
    &DoNothing_keymap,          /* RemnantKeys */
    &LeftAltMode_keymap,        /* LeftAlt */
    &LeftModMode_keymap,        /* LeftMod */
    &RightAltMode_keymap,       /* RightAlt */
    &RightModMode_keymap,       /* RightMod */
    &AltTab_keymap,             /* AltTab */
    &WindowSnap_keymap,         /* WindowSnap */
    &NumPad_keymap,             /* NumPad */
    &GamingEntryPoint_keymap,   /* GamingNoKeys */
    &GamingBacktick_keymap,     /* GamingBacktick */
    &GamingTab_keymap,          /* GamingTab */
    &GamingCapsLock_keymap,     /* GamingCapsLock */
    &GamingSpace_keymap,        /* GamingSpace */
};

// ****************************************************************************
// Variables
// ****************************************************************************

KeyboardLayout CurrentLayout = dvorak;
Mode EntryPointMode = NoKeys;
Mode CurrentMode = NoKeys;
OSMode CurrentOSMode = Windows;
ModeState CurrentModeState = Clean;

// ****************************************************************************
// State Dependant Values
// ****************************************************************************

uint8_t CapsLockMod() {
    switch(CurrentOSMode){
        case Windows: return LCtrl;
        case OSX: return LGui;
    }
}

uint8_t AppSwitchModifierKeycode(Side side) {
    switch (side) {
        case Left:
            switch (CurrentOSMode){
                case Windows: return LAlt;
                case OSX: return LGui;
            }
        case Right:
            switch (CurrentOSMode){
                case Windows: return RAlt;
                case OSX: return RGui;
            }
    }
}

uint8_t WindowSnapModifierKeycode() {
    switch(CurrentOSMode){
        case Windows: return LCtrl | LGui;
        case OSX: return LCtrl | LGui | LShift;
    }
}

// ****************************************************************************
// Mode implementations
// ****************************************************************************

ControlCode Configuration_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
     // map modifier
    if (i == 0) switch (inbuf[0]) {
        case LCtrl:
        case RCtrl:
            SetOSMode(Windows);
            return Continue;
        case LGui:
        case RGui:
            SetOSMode(OSX);
            return Continue;
    }

     // map key
    if (i >= 2) switch (inbuf[i]){
        case _Escape: return Continue;
        case _F1:
            CurrentModeState = Used;
            CurrentLayout = qwerty;
            EntryPointMode = NoKeys;
            return Continue;
        case _F2:
            CurrentModeState = Used;
            CurrentLayout = dvorak;
            EntryPointMode = NoKeys;
            return Continue;
        case _F3:
            CurrentModeState = Used;
            CurrentLayout = qwerty;
            EntryPointMode = GamingNoKeys;
            return Continue;
    }
}

ControlCode EntryPoint_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // Modifier entry points
    switch (inbuf[0]){
        case LAlt: return EnterMode(LeftAlt, Clean);
        case RAlt: return EnterMode(RightAlt, Clean);
    }

    // normal key entry points
    switch (inbuf[2]){
        case _Escape:   return EnterMode(Configuration, Clean);
    }

    // No special behavior activated. Apply normal keyboard behavior.
    return EnterMode(Normal, Used);
}

// a keymap that always does nothing
ControlCode DoNothing_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    return InvalidKey();
}

ControlCode mapNormalKeyToCurrentLayout(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) {
        return SendModifiers(inbuf[0], outbuf);
    }
    // map key
    if (i >= 2) {
        uint8_t inkey = inbuf[i];
        switch (inkey){
            case _CapsLock:         return SendModifiers(CapsLockMod(), outbuf);
        }
        // lookup key for current keyboard layout
        if (inkey >= _A && inkey <= _CapsLock){
            uint8_t outkey = Keymap[CurrentLayout][inkey-4];
            return SendKey(outkey, outbuf);
        }

        return SendKey(inkey, outbuf);
    }
}

// the keymap that just maps the key to the current keyboard layout
ControlCode Normal_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
}

ControlCode LeftAltMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) switch (inbuf[i]) {
        case LAlt:           return Continue;
    }
    // map 1st key
    if (i == 2) switch (inbuf[i]) {
        // normal mode modifiers
        case _A:             return EnterMode(LeftMod, Used);
        case _S:             return EnterMode(LeftMod, Used);
        case _D:             return EnterMode(LeftMod, Used);
        case _F:             return EnterMode(LeftMod, Used);
        // map secondary modifier
        case _X:             return EnterMode(NumPad, Used);
        case _C:             return EnterMode(WindowSnap, Used);
    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
        // alt mode modifiers
        case _Q:             return SendModifiers(LShift, outbuf);
        case _W:             return SendModifiers(LAlt, outbuf);
        case _E:             return SendModifiers(LCtrl, outbuf);
        case _R:             return SendModifiers(LGui, outbuf);

        // Left Hand keys
        case _Tab:           return EnterMode(AltTab, Used);
        case _1:             return SendKey(_F1, outbuf);
        case _2:             return SendKey(_F2, outbuf);
        case _3:             return SendKey(_F3, outbuf);
        case _4:             return SendKey(_F4, outbuf);
        case _5:             return SendKey(_F5, outbuf);
        case _6:             return SendKey(_F6, outbuf);

        // Right Hand keys
        case _Y:             return SendKey(_Escape, outbuf);
        case _U:             return SendKey(_Home, outbuf);
        case _I:             return SendKey(_PgUp, outbuf);
        case _O:             return SendKey(_PgDn, outbuf);
        case _P:             return SendKey(_End, outbuf);
        case _LeftBracket:   return SendKey(_Enter, outbuf);
        case _RightBracket:  return SendKey(_Menu, outbuf);
        case _Backslash:     return EnterMode(AltTab, Used);
        case _Backspace:     return SendKey(_CapsLock, outbuf);
        case _H:             return SendKey(_Backspace, outbuf);
        case _J:             return SendKey(_Left, outbuf);
        case _K:             return SendKey(_Up, outbuf);
        case _L:             return SendKey(_Down, outbuf);
        case _Semicolon:     return SendKey(_Right, outbuf);
        case _Apostrophe:    return SendKey(_Delete, outbuf);
        case _7:             return SendKey(_F7, outbuf);
        case _8:             return SendKey(_F8, outbuf);
        case _9:             return SendKey(_F9, outbuf);
        case _0:             return SendKey(_F10, outbuf);
        case _Dash:          return SendKey(_F11, outbuf);
        case _Equals:        return SendKey(_F12, outbuf);
    }
    // all other keys
    return EnterMode(Normal, Used);
}

ControlCode LeftModMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // return to LeftAlt Mode when LAlt is the only key pressed
    if (inbuf[0] == LAlt && NumKeysOrModsPressed(inbuf) == 1)
        return EnterMode(LeftAlt, Used);

    // map modifier
    if (i == 0) switch (inbuf[i]) {
        case LAlt:           return Continue;
    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
        // normal mode modifiers
        case _A:             return SendModifiers(LShift, outbuf);
        case _S:             return SendModifiers(LAlt, outbuf);
        case _D:             return SendModifiers(LCtrl, outbuf);
        case _F:             return SendModifiers(LGui, outbuf);
    }
    // all other keys
    return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
}

ControlCode RightAltMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) switch (inbuf[i]) {
        case RAlt:           return Continue;
    }
    // map first key
    if (i == 2) switch (inbuf[i]) {
        // normal mode modifiers
        case _J:             return EnterMode(RightMod, Used);
        case _K:             return EnterMode(RightMod, Used);
        case _L:             return EnterMode(RightMod, Used);
        case _Semicolon:     return EnterMode(RightMod, Used);
    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
         // alt mode modifiers
        case _U:             return SendModifiers(RGui, outbuf);
        case _I:             return SendModifiers(RCtrl, outbuf);
        case _O:             return SendModifiers(RAlt, outbuf);
        case _P:             return SendModifiers(RShift, outbuf);
        // Left Hand keys
        case _Tab:           return EnterMode(AltTab, Used);
        case _1:             return SendKey(_F1, outbuf);
        case _2:             return SendKey(_F2, outbuf);
        case _3:             return SendKey(_F3, outbuf);
        case _4:             return SendKey(_F4, outbuf);
        case _5:             return SendKey(_F5, outbuf);
        case _6:             return SendKey(_F6, outbuf);
        // Right Hand keys
        case _Backslash:     return EnterMode(AltTab, Used);
    }
    // all other keys
    return EnterMode(Normal, Used);
}

ControlCode RightModMode_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // return to RightAlt Mode when RAlt is the only key pressed
    if (inbuf[0] == RAlt && NumKeysOrModsPressed(inbuf) == 1)
        return EnterMode(RightAlt, Used);

    // map modifier
    if (i == 0) switch (inbuf[0]){
        case RAlt:           return Continue;
    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
        // normal mode modifiers
        case _J:             return SendModifiers(RGui, outbuf);
        case _K:             return SendModifiers(RCtrl, outbuf);
        case _L:             return SendModifiers(RAlt, outbuf);
        case _Semicolon:     return SendModifiers(RShift, outbuf);
    }
    // all other keys
    return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
}

ControlCode AltTab_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) {
        uint8_t Mods = 0;
        if (inbuf[0] & LAlt) Mods |= AppSwitchModifierKeycode(Left);
        if (inbuf[0] & RAlt) Mods |= AppSwitchModifierKeycode(Right);
        if (inbuf[0] & LShift) Mods |= LShift;
        if (inbuf[0] & RShift) Mods |= RShift;
        return SendModifiers(Mods, outbuf);
    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
        // Tab
        case _Tab:           return SendKey(_Tab, outbuf);
        case _Backslash:     return SendKey(_Tab, outbuf);
        // Shift
        case _Q:             return SendModifiers(LShift, outbuf);
        case _P:             return SendModifiers(RShift, outbuf);
        // arrow keys
        case _J:             return SendKey(_Left, outbuf);
        case _K:             return SendKey(_Up, outbuf);
        case _L:             return SendKey(_Down, outbuf);
        case _Semicolon:     return SendKey(_Right, outbuf);
    }
    // all other keys
    return InvalidKey();
}

ControlCode WindowSnap_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // exit condition: first key pressed is no longer _C
    if (inbuf[2] != _C) return EnterMode(NoKeys, Used);

    // map modifier
    if (i == 0) switch (inbuf[i]) {
        case LAlt:         return Continue;
    }
    // map first key
    if (i == 2) switch (inbuf[i]) { // must be _C because of exit guard
        default:           return SendModifiers(WindowSnapModifierKeycode(), outbuf);
    }
    // map subsequent keys
    if (i > 2) switch (inbuf[i]) {
        default:           return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
    }
}

ControlCode NumPad_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // exit condition: first key pressed is no longer _X
    if (inbuf[2] != _X)             return EnterMode(NoKeys, Used);

    // map modifier
    if (i == 0) switch (inbuf[i]) {
        case LAlt:                  return Continue;
        default:                    return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
    }
    // map first key
    if (i == 2) switch (inbuf[i]) { // must be _X because of exit guard
        default:                    return Continue;
    }
    // map subsequent keys
    if (i > 2)  switch (inbuf[i]) {
        case _7:                    return SendKey(_Numpad7, outbuf);
        case _8:                    return SendKey(_Numpad8, outbuf);
        case _9:                    return SendKey(_Numpad9, outbuf);
        case _0:                    return SendKey(_NumpadTimes, outbuf);
        case _U:                    return SendKey(_Numpad4, outbuf);
        case _I:                    return SendKey(_Numpad5, outbuf);
        case _O:                    return SendKey(_Numpad6, outbuf);
        case _P:                    return SendKey(_NumpadMinus, outbuf);
        case _LeftBracket:          return SendKey(_NumpadEnter, outbuf);
        case _RightBracket:         return SendKey(_NumLock, outbuf);
        case _H:                    return SendKey(_Backspace, outbuf);
        case _J:                    return SendKey(_Numpad1, outbuf);
        case _K:                    return SendKey(_Numpad2, outbuf);
        case _L:                    return SendKey(_Numpad3, outbuf);
        case _Semicolon:            return SendKey(_NumpadPlus, outbuf);
        case _Apostrophe:           return SendKeyCombo(LShift, _Dash, outbuf); // Underscore
        case _N:                    return SendKeyCombo(LShift, _Semicolon, outbuf); // Colon
        case _M:                    return SendKey(_Numpad0, outbuf);
        case _Comma:                return SendKey(_Comma, outbuf);
        case _Fullstop:             return SendKey(_Fullstop, outbuf);
        case _ForwardSlash:         return SendKey(_NumpadDivide, outbuf);
        case _Space:                return SendKey(_Space, outbuf);
        case _Enter:                return SendKey(_Enter, outbuf);
    }
    // all other keys
    return InvalidKey();
}

// ================== Gaming Mode ===================

ControlCode GamingEntryPoint_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) switch (inbuf[0]) {
        case LGui:        return SendKey(_Enter, outbuf);
    }
    // map first key
    if (i == 2) switch (inbuf[i]) {
        case _Escape:     return EnterMode(Configuration, Clean);
        case _Backtick:   return EnterMode(GamingBacktick, Clean);
        case _Tab:        return EnterMode(GamingTab, Clean);
        case _CapsLock:   return EnterMode(GamingCapsLock, Clean);
        case _Space:      return EnterMode(GamingSpace, Clean);
    }
    // all other keys
    return mapNormalKeyToCurrentLayout(inbuf, i, outbuf);
}


ControlCode GamingBacktick_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) switch (inbuf[0]) {

    }
    // map any key
    if (i >= 2) switch (inbuf[i]) {
        case _Backtick:      return Continue;
        case _Space:         return SendModifiers(LCtrl, outbuf);
        // backtick + row0 number ==> LH function key
        case _1:             return SendKey(_F1, outbuf);
        case _2:             return SendKey(_F2, outbuf);
        case _3:             return SendKey(_F3, outbuf);
        case _4:             return SendKey(_F4, outbuf);
        case _5:             return SendKey(_F5, outbuf);
        case _6:             return SendKey(_F6, outbuf);
    }
    // all other keys
    return InvalidKey();
}

ControlCode GamingTab_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
     // map modifier
    if (i == 0) switch (inbuf[0]) {

    }
    // map any key
    if (i >= 2) switch (inbuf[i]){
        case _Tab:           return Continue;
        case _Space:         return SendModifiers(LCtrl, outbuf);
        // Tab + row0 number ==> shift + LH number
        case _1:             return SendKeyCombo(LShift, _1, outbuf);
        case _2:             return SendKeyCombo(LShift, _2, outbuf);
        case _3:             return SendKeyCombo(LShift, _3, outbuf);
        case _4:             return SendKeyCombo(LShift, _4, outbuf);
        case _5:             return SendKeyCombo(LShift, _5, outbuf);
        case _6:             return SendKeyCombo(LShift, _6, outbuf);
        // Tab + row1 letter ==> shift + RH number
        case _Q:             return SendKeyCombo(LShift, _6, outbuf);
        case _W:             return SendKeyCombo(LShift, _7, outbuf);
        case _E:             return SendKeyCombo(LShift, _8, outbuf);
        case _R:             return SendKeyCombo(LShift, _9, outbuf);
        case _T:             return SendKeyCombo(LShift, _0, outbuf);
    }
    // all other keys
    return InvalidKey();
}


ControlCode GamingCapsLock_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
     // map modifier
    if (i == 0) switch (inbuf[0]) {

    }
    // map any key
    if (i >= 2) switch (inbuf[i]){
        case _CapsLock:      return Continue;
        case _Space:         return SendModifiers(LCtrl, outbuf);
        // CapsLock + row1 letter ==> RH number
        case _Q:             return SendKey(_6, outbuf);
        case _W:             return SendKey(_7, outbuf);
        case _E:             return SendKey(_8, outbuf);
        case _R:             return SendKey(_9, outbuf);
        case _T:             return SendKey(_0, outbuf);
    }
    // all other keys
    return InvalidKey();
}

ControlCode GamingSpace_keymap(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    // map modifier
    if (i == 0) switch (inbuf[0]) {
        // Space + Shift ==> RH function key (F7)
        case LShift:       return SendKey(_F7, outbuf);
        case LAlt:         return SendKey(_Space, outbuf);
    }
    // map any key
    if (i >= 2) switch (inbuf[i]){
        case _Space:         return Continue;
        case _Backtick:      return EnterMode(GamingBacktick, Used);
        case _Tab:           return EnterMode(GamingTab, Used);
        case _CapsLock:      return EnterMode(GamingCapsLock, Used);
         // Space + row0 number ==> ctrl + LH number
        case _1:             return SendKeyCombo(LCtrl, _1, outbuf);
        case _2:             return SendKeyCombo(LCtrl, _2, outbuf);
        case _3:             return SendKeyCombo(LCtrl, _3, outbuf);
        case _4:             return SendKeyCombo(LCtrl, _4, outbuf);
        case _5:             return SendKeyCombo(LCtrl, _5, outbuf);
        case _6:             return SendKeyCombo(LCtrl, _6, outbuf);
        // Space + R1,R2 letter keys ==> navigation keys
        case _Q:             return SendKey(_Delete, outbuf);
        case _W:             return SendKey(_PgUp, outbuf);
        case _E:             return SendKey(_Up, outbuf);
        case _R:             return SendKey(_PgDn, outbuf);
        case _T:             return SendKey(_Backspace, outbuf);
        case _A:             return SendKey(_Home, outbuf);
        case _S:             return SendKey(_Left, outbuf);
        case _D:             return SendKey(_Down, outbuf);
        case _F:             return SendKey(_Right, outbuf);
        case _G:             return SendKey(_End, outbuf);
        // Space + R3 letter keys ==> RH function keys
        case _Z:             return SendKey(_F8, outbuf);
        case _X:             return SendKey(_F9, outbuf);
        case _C:             return SendKey(_F10, outbuf);
        case _V:             return SendKey(_F11, outbuf);
        case _B:             return SendKey(_F12, outbuf);

    }
    // all other keys
    return InvalidKey();
}

// ****************************************************************************
// State Handling Callbacks
// ****************************************************************************

void HandleLastKeyReleased() {
    // send normal keys on release of custom modifier if no other keys were pressed while it was held down
    if (CurrentModeState == Clean) switch (CurrentMode) {
        case Configuration: // send Escape on release
            PressAndReleaseKey((RichKey){ 0, _Escape } );
            break;
        case LeftAlt: // send Left Alt on release
            PressAndReleaseKey((RichKey){ LAlt, 0 } );
            break;
        case RightAlt: // send Right Alt on release
            PressAndReleaseKey((RichKey){ RAlt, 0 } );
            break;
        case GamingBacktick: // send Backtick on release
            PressAndReleaseKey((RichKey){ 0, _Backtick } );
            break;
        case GamingTab: // send Tab on release
            PressAndReleaseKey((RichKey){ 0, _Tab } );
            break;
        case GamingSpace: // send Space on release
            PressAndReleaseKey((RichKey){ 0, _Space } );
            break;
    }
    SetMode(EntryPointMode, Clean);
}

// ****************************************************************************
// Helper Functions
// ****************************************************************************

void LoadOSMode() {
    OSMode osMode = Windows;
    EEPROM.get( OSModeSlot, osMode );
    CurrentOSMode = osMode;
}

void SetOSMode(OSMode osMode) {
    CurrentOSMode = osMode;
    EEPROM.put( OSModeSlot, osMode );
}

void SetMode(Mode mode, ModeState modeState) {
    CurrentMode = mode;
    CurrentModeState = modeState;
}

ControlCode EnterMode(Mode mode, ModeState modeState) {
    Log("entering Mode: " + GetModeString(mode));
    SetMode(mode, modeState);
    return Restart;
}

ControlCode SendKey(uint8_t keycode, uint8_t outbuf[8]) {
    return SendKeyCombo(0, keycode, outbuf);
}

ControlCode SendModifiers(uint8_t mods, uint8_t outbuf[8]) {
    return SendKeyCombo(mods, 0, outbuf);
}

ControlCode SendKeyCombo(uint8_t mods, uint8_t keycode, uint8_t outbuf[8]) {
    CurrentModeState = Used;
    MergeKeyIntoBuffer((RichKey){ mods, keycode }, outbuf);
    return Continue;
}

ControlCode InvalidKey() {
    CurrentModeState = Used;
    return Stop;
}

// map key presses according to the current mode
ControlCode MapKey(uint8_t inbuf[8], uint8_t i, uint8_t outbuf[8]) {
    return KeyMaps[CurrentMode](inbuf, i, outbuf);
}

uint8_t NumKeysPressed(uint8_t buf[8]) {
    uint8_t count = 0;
    for (uint8_t i=2; i<8; i++) {
        if (buf[i]) count++;
    }
    return count;
}

uint8_t NumModsPressed(uint8_t buf[8]) {
    uint8_t bitset = buf[0];
    uint8_t count;
    for (count = 0; bitset; count++)
        bitset &= bitset - 1;
    return count;
}

uint8_t NumKeysOrModsPressed(uint8_t buf[8]) {
    return NumModsPressed(buf) + NumKeysPressed(buf);
}

String GetOSModeString(OSMode osMode) {
    switch (osMode){
        case Windows:    return "Win";
        case OSX:        return "OSX";
    }
}

String GetModeString(Mode mode) {
    switch (mode){
        case Configuration:   return "Configuration";
        case NoKeys:          return "NoKeys";
        case Normal:          return "Normal";
        case RemnantKeys:     return "RemnantKeys";
        case LeftAlt:         return "LeftAlt";
        case LeftMod:         return "LeftMod";
        case RightAlt:        return "RightAlt";
        case RightMod:        return "RightMod";
        case AltTab:          return "AltTab";
        case WindowSnap:      return "WindowSnap";
        case NumPad:          return "NumPad";
        case GamingNoKeys:    return "GamingNoKeys";
        case GamingBacktick:  return "GamingBacktick";
        case GamingTab:       return "GamingTab";
        case GamingCapsLock:  return "GamingCapsLock";
        case GamingSpace:     return "GamingSpace";
        default:              return "<unknown>";
    }
}

String GetModeStateString(ModeState modeState) {
    return (modeState == Used) ? "*" : "";
}

String GetLayoutString(KeyboardLayout layout) {
    switch (layout){
        case qwerty:    return "QY";
        case dvorak:    return "DV";
        case colemak:   return "CM";
    }
}

// ****************************************************************************
// Shared Function Implementations
// ****************************************************************************

void InitializeState() {
    LoadOSMode();
}

void TransformBuffer(uint8_t inbuf[8], uint8_t outbuf[8]) {
    if (NumKeysOrModsPressed(inbuf) == 0) {
        HandleLastKeyReleased();
    } else {
        int i = 0;
        while (i < 8) {
            if (i==1 || !inbuf[i]) {
                i++;
                continue;
            }
            switch (MapKey(inbuf, i, outbuf)) {
                case Continue: i++; break;
                case Stop: i=8; break;
                case Restart: i=0; break;
            }
        }
    }
}

String GetStateString() {
    String spaces = "                              ";
    String stateStr = "[" + GetOSModeString(CurrentOSMode) + "." + GetLayoutString(CurrentLayout) + "." + GetModeString(CurrentMode) + GetModeStateString(CurrentModeState) + "]";
    String neededSpaces = spaces.substring(0, 26 - stateStr.length());

    return stateStr + neededSpaces;
}
