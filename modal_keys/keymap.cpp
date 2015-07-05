#include "keymap.h"

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
    Normal,
    RemnantKeys,
    LeftAlt,
    LeftFunc,
    LeftMod,
    RightAlt,
    RightFunc,
    RightMod,
    AltTab,
    WindowSnap,
    NumPad,
    Configuration
} Mode;

typedef enum {
    Left = 0,
    Right
} Side;

// ****************************************************************************
// Function Declarations
// ****************************************************************************

// pure functions
RichKey mapLeftFuncKey(uint8_t *buf, uint8_t i);
RichKey mapRightFuncKey(uint8_t *buf, uint8_t i);

// state changing methods
RichKey DoNothing_keymap(uint8_t *buf, uint8_t i);
RichKey Normal_keymap(uint8_t *buf, uint8_t i);
RichKey EntryPoint_keymap(uint8_t *buf, uint8_t i);
RichKey LeftAltMode_keymap(uint8_t *buf, uint8_t i);
RichKey LeftFuncMode_keymap(uint8_t *buf, uint8_t i);
RichKey LeftModMode_keymap(uint8_t *buf, uint8_t i);
RichKey RightAltMode_keymap(uint8_t *buf, uint8_t i);
RichKey RightFuncMode_keymap(uint8_t *buf, uint8_t i);
RichKey RightModMode_keymap(uint8_t *buf, uint8_t i);
RichKey AltTab_keymap(uint8_t *buf, uint8_t i);
RichKey WindowSnap_keymap(uint8_t *buf, uint8_t i);
RichKey NumPad_keymap(uint8_t *buf, uint8_t i);
RichKey Configuration_keymap(uint8_t *buf, uint8_t i);

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
    &Normal_keymap,             /* Normal */
    &DoNothing_keymap,          /* RemnantKeys */
    &LeftAltMode_keymap,        /* LeftAlt */
    &LeftFuncMode_keymap,       /* LeftFunc */
    &LeftModMode_keymap,        /* LeftMod */
    &RightAltMode_keymap,       /* RightAlt */
    &RightFuncMode_keymap,      /* RightFunc */
    &RightModMode_keymap,       /* RightMod */
    &AltTab_keymap,             /* AltTab */
    &WindowSnap_keymap,         /* WindowSnap */
    &NumPad_keymap,             /* NumPad */
    &Configuration_keymap,      /* Configuration */
};

// ****************************************************************************
// Variables
// ****************************************************************************

KeyboardLayout CurrentLayout = dvorak;
Mode CurrentMode = NoKeys;
OSMode CurrentOSMode = Windows;

// ****************************************************************************
// State Dependant Values
// ****************************************************************************

RichKey CapsLockKey() {
    switch(CurrentOSMode){
        case Windows: return (RichKey){ LCtrl, 0 };
        case OSX: return (RichKey){ LGui, 0 };
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
// Helper Functions
// ****************************************************************************

bool numKeysPressed(uint8_t *buf){
    uint8_t count = 0;
    for (uint8_t i=2; i<8; i++) {
        if (buf[i]) count++;
    }
    return count;
}

bool numModsPressed(uint8_t *buf){
    uint8_t bitset = buf[0];
    uint8_t count;
    for (count = 0; bitset; count++)
        bitset &= bitset - 1;
    return count;
}

bool numKeysOrModsPressed(uint8_t *buf){
    return numModsPressed(buf) + numKeysPressed(buf);
}

RichKey AltTabKey(uint8_t *buf, Side side){
    if (numKeysPressed(buf) == 1){
        CurrentMode = AltTab;
        return (RichKey){ AppSwitchModifierKeycode(side), _Tab };
    } else return (RichKey){ 0, _Tab };
}

bool mapModifier(uint8_t mod, Mode newMode, RichKey* out){
    *out = (RichKey){ mod, 0 };
    CurrentMode = newMode;
    return true;
}

String GetOSModeString(){
    switch (CurrentOSMode){
        case Windows:    return "Win";
        case OSX:        return "OSX";
    }
}

String GetModeString(){
    switch (CurrentMode){
        case NoKeys:          return "NoKeys";
        case Normal:          return "Normal";
        case RemnantKeys:     return "RemnantKeys";
        case LeftAlt:         return "LeftAlt";
        case LeftFunc:        return "LeftFunc";
        case LeftMod:         return "LeftMod";
        case RightAlt:        return "RightAlt";
        case RightFunc:       return "RightFunc";
        case RightMod:        return "RightMod";
        case AltTab:          return "AltTab";
        case WindowSnap:      return "WindowSnap";
        case NumPad:          return "NumPad";
        case Configuration:   return "Configuration";
        default:              return "<unknown>";
    }
}
String GetLayoutString(){
    switch (CurrentLayout){
        case qwerty:    return "QY";
        case dvorak:    return "DV";
        case colemak:   return "CM";
    }
}

// ****************************************************************************
// Mode implementations
// ****************************************************************************

// a keymap that always does nothing
RichKey DoNothing_keymap(uint8_t *buf, uint8_t i){
    return NoKey;
}

RichKey mapNormalKeyToCurrentLayout(uint8_t *buf, uint8_t i){
    // map modifier
    if (i == 0){
        return (RichKey){ buf[0], 0 };
    }
    // map key
    else {
        uint8_t inkey = buf[i];
        switch (inkey){
            case _CapsLock:         return CapsLockKey();
        }
        // lookup key for current keyboard layout
        if (inkey >= _A && inkey <= _CapsLock){
            uint8_t outkey = Keymap[CurrentLayout][inkey-4];
            return (RichKey){ 0, outkey };
        }

        return (RichKey){ 0, inkey };
    }
}

// the keymap that just maps the key to the current keyboard layout
RichKey Normal_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = Normal;
    return mapNormalKeyToCurrentLayout(buf, i);
}

RichKey EntryPoint_keymap(uint8_t *buf, uint8_t i){
    // Modifier entry points
    switch (buf[0]){
        case LAlt: return LeftAltMode_keymap(buf, i);
        case RAlt: return RightAltMode_keymap(buf, i);
    }

    // normal key entry points
    switch (buf[2]){
        case _Backtick: return Configuration_keymap(buf, i);
    }

    // No special behavior activated. Apply normal keyboard behavior.
    return Normal_keymap(buf, i);
}

bool mapLAltModifier(uint8_t key, RichKey* out){
    switch (key){
        // func mode modifiers
        case _Q:             return mapModifier(LShift, LeftFunc, out);
        case _W:             return mapModifier(LAlt, LeftFunc, out);
        case _E:             return mapModifier(LCtrl, LeftFunc, out);
        case _R:             return mapModifier(LGui, LeftFunc, out);

        // normal mode modifiers
        case _A:             return mapModifier(LShift, LeftMod, out);
        case _S:             return mapModifier(LAlt, LeftMod, out);
        case _D:             return mapModifier(LCtrl, LeftMod, out);
        case _F:             return mapModifier(LGui, LeftMod, out);
    }
    return false;
}

RichKey LeftAltMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = LeftAlt;
    RichKey funcKey = mapLeftFuncKey(buf, 2);
    // map modifier
    if (i == 0) switch (buf[i]){
        case LAlt:
            if (numKeysPressed(buf) == 0)
                return NoKey; // don't press LAlt by default - use it as a custom modifier
            else if (funcKey == NoKey){ // a non-func-mode key was pressed first
                // press Alt key slightly earlier than other keys
                PressKey((RichKey){ LAlt, 0 });
            }
    }
    // map secondary modifier
    else if (i == 2) switch (buf[i]){
        case _X:           return NumPad_keymap(buf, i);
        case _C:           return WindowSnap_keymap(buf, i);
    }

    // map key
    if (funcKey == NoKey){ // a non-func-mode key was pressed first
        // Switch to Normal keyboard where Alt key behaves like Alt key
        return Normal_keymap(buf, i);
    }
    else {
        return LeftFuncMode_keymap(buf, i);
    }
}

RichKey mapLeftFuncKey(uint8_t *buf, uint8_t i){
    RichKey out;
    // map modifier
    if (i == 0) switch (buf[0]){
        case LAlt:
            return NoKey;
    }
    // map key
    else if (mapLAltModifier(buf[i], &out))
        return out;
    else switch (buf[i]){
        // AltTab
        case _Tab:           return AltTabKey(buf, Left);
        case _X:             return NumPad_keymap(buf, i);
        case _C:             return WindowSnap_keymap(buf, i);
        case _Y:             return (RichKey){ 0, _Escape };
        case _U:             return (RichKey){ 0, _Home };
        case _I:             return (RichKey){ 0, _PgUp };
        case _O:             return (RichKey){ 0, _PgDn };
        case _P:             return (RichKey){ 0, _End };
        case _LeftBracket:   return (RichKey){ 0, _Enter };
        case _RightBracket:  return (RichKey){ 0, _Menu };
        case _Backslash:     return AltTabKey(buf, Left);
        case _Backspace:     return (RichKey){ 0, _CapsLock };
        case _H:             return (RichKey){ 0, _Backspace };
        case _J:             return (RichKey){ 0, _Left };
        case _K:             return (RichKey){ 0, _Up };
        case _L:             return (RichKey){ 0, _Down };
        case _Semicolon:     return (RichKey){ 0, _Right };
        case _Apostrophe:    return (RichKey){ 0, _Delete };
        case _7:             return (RichKey){ 0, _F7 };
        case _8:             return (RichKey){ 0, _F8 };
        case _9:             return (RichKey){ 0, _F9 };
        case _0:             return (RichKey){ 0, _F10 };
        case _Dash:          return (RichKey){ 0, _F11 };
        case _Equals:        return (RichKey){ 0, _F12 };
    }
    return NoKey;
}

RichKey LeftFuncMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = LeftFunc;
    return mapLeftFuncKey(buf, i);
}

RichKey LeftModMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = LeftMod;
    RichKey out;
    // map modifier
    if (i == 0) switch (buf[0]){
        case LAlt:
            if (numKeysPressed(buf) == 0)
                CurrentMode = LeftFunc;
            return NoKey;
    }
    // map key
    else if (mapLAltModifier(buf[i], &out))
        return out;
    return mapNormalKeyToCurrentLayout(buf, i);
}

bool mapRAltModifier(uint8_t key, RichKey* out){
    switch (key){
        // func mode modifiers
        case _U:             return mapModifier(RGui, RightFunc, out);
        case _I:             return mapModifier(RCtrl, RightFunc, out);
        case _O:             return mapModifier(RAlt, RightFunc, out);
        case _P:             return mapModifier(RShift, RightFunc, out);

        // normal mode modifiers
        case _J:             return mapModifier(RGui, RightMod, out);
        case _K:             return mapModifier(RCtrl, RightMod, out);
        case _L:             return mapModifier(RAlt, RightMod, out);
        case _Semicolon:     return mapModifier(RShift, RightMod, out);
    }
    return false;
}

RichKey RightAltMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = RightAlt;
    RichKey funcKey = mapRightFuncKey(buf, 2);
    // map modifier
    if (i == 0) switch (buf[0]){
        case RAlt:
            if (numKeysPressed(buf) == 0)
                return NoKey; // don't press RAlt by default - use it as a custom modifier
            else if (funcKey == NoKey){ // a non-func-mode key was pressed first
                // press Alt key slightly earlier than other keys
                PressKey((RichKey){ RAlt, 0 });
            }
    }
    if (funcKey == NoKey) // a non-func-mode key was pressed first
        // Switch to Normal keyboard where Alt key behaves like Alt key
        return Normal_keymap(buf, i);
    else
        return RightFuncMode_keymap(buf, i);
}

RichKey mapRightFuncKey(uint8_t *buf, uint8_t i){
    RichKey out;
    // map modifier
    if (i == 0) switch (buf[0]){
        case RAlt:
            return NoKey;
    }
    // map key
    else if (mapRAltModifier(buf[i], &out))
        return out;
    else switch (buf[i]){
        // AltTab
        case _Tab:           return AltTabKey(buf, Right);
        case _Backslash:     return AltTabKey(buf, Right);
        case _1:             return (RichKey){ 0, _F1 };
        case _2:             return (RichKey){ 0, _F2 };
        case _3:             return (RichKey){ 0, _F3 };
        case _4:             return (RichKey){ 0, _F4 };
        case _5:             return (RichKey){ 0, _F5 };
        case _6:             return (RichKey){ 0, _F6 };
    }
    return NoKey;
}

RichKey RightFuncMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = RightFunc;
    return mapRightFuncKey(buf, i);
}

RichKey RightModMode_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = RightMod;
    RichKey out;
    // map modifier
    if (i == 0) switch (buf[0]){
        case RAlt:
            if (numKeysPressed(buf) == 0)
                CurrentMode = RightFunc;
            return NoKey;
    }
    // map key
    else if (mapRAltModifier(buf[i], &out))
        return out;
    return mapNormalKeyToCurrentLayout(buf, i);
}

RichKey AltTab_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = AltTab;
    // map modifier
    if (i == 0) {
        uint8_t Mods = 0;
        if (buf[0] & LAlt) Mods |= AppSwitchModifierKeycode(Left);
        if (buf[0] & RAlt) Mods |= AppSwitchModifierKeycode(Right);
        if (buf[0] & LShift) Mods |= LShift;
        if (buf[0] & RShift) Mods |= RShift;
        return (RichKey){ Mods, 0 };
    }
    // map key
    else switch (buf[i]){
        // AltTab
        case _Tab:
        case _Backslash:
            return (RichKey){ 0, _Tab };
        case _Q:
            return (RichKey){ LShift, 0 };
        case _P:
            return (RichKey){ RShift, 0 };
    }
    return NoKey;
}

RichKey WindowSnap_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = WindowSnap;

    // exit condition: first key pressed is no longer _C
    if (buf[2] != _C) return EntryPoint_keymap(buf, i);

    // map modifier
    if (i == 0) {
        return NoKey;
    }
    // map first key
    else if (i == 2) { // must be _C because of exit guard
        return (RichKey){ WindowSnapModifierKeycode(), 0, _CustomModifier };
    }
    // map subsequent keys
    else {
        return mapNormalKeyToCurrentLayout(buf, i);
    }
}

RichKey NumPad_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = NumPad;

    // exit condition: first key pressed is no longer _X
    if (buf[2] != _X) return EntryPoint_keymap(buf, i);

    // map modifier
    if (i == 0) {
        return NoKey;
    }
    // map first key
    else if (i == 2) { // must be _X because of exit guard
        return CustomModifierKey;
    }
    // map subsequent keys
    else switch (buf[i]){
        case _7:                    return (RichKey){ 0, _Numpad7 };
        case _8:                    return (RichKey){ 0, _Numpad8 };
        case _9:                    return (RichKey){ 0, _Numpad9 };
        case _0:                    return (RichKey){ 0, _NumpadTimes };
        case _U:                    return (RichKey){ 0, _Numpad4 };
        case _I:                    return (RichKey){ 0, _Numpad5 };
        case _O:                    return (RichKey){ 0, _Numpad6 };
        case _P:                    return (RichKey){ 0, _NumpadMinus };
        case _LeftBracket:          return (RichKey){ 0, _NumpadEnter };
        case _RightBracket:         return (RichKey){ 0, _NumLock };
        case _H:                    return (RichKey){ 0, _Backspace };
        case _J:                    return (RichKey){ 0, _Numpad1 };
        case _K:                    return (RichKey){ 0, _Numpad2 };
        case _L:                    return (RichKey){ 0, _Numpad3 };
        case _Semicolon:            return (RichKey){ 0, _NumpadPlus };
        case _Apostrophe:           return (RichKey){ LShift, _Dash }; // Underscore
        case _N:                    return (RichKey){ LShift, _Semicolon }; // Colon
        case _M:                    return (RichKey){ 0, _Numpad0 };
        case _Comma:                return (RichKey){ 0, _Comma };
        case _Fullstop:             return (RichKey){ 0, _Fullstop };
        case _ForwardSlash:         return (RichKey){ 0, _NumpadDivide };
        case _Space:                return (RichKey){ 0, _Space };
        case _Enter:                return (RichKey){ 0, _Enter };
    }
    return NoKey;
}

RichKey Configuration_keymap(uint8_t *buf, uint8_t i){
    CurrentMode = Configuration;
     // map modifier
    if (i == 0) switch (buf[0]) {
        case LCtrl:
        case RCtrl:
            CurrentOSMode = Windows;
            break;
        case LGui:
        case RGui:
            CurrentOSMode = OSX;
            break;
    }

     // map key
    if (i >= 2) switch (buf[i]){
        case _Backtick: return NoKey;
        case _1:
            CurrentLayout = qwerty;
            break;
        case _2:
            CurrentLayout = dvorak;
            break;
        case _3:
            CurrentLayout = colemak;
            break;
    }
    CurrentMode = RemnantKeys;
    return NoKey;
}

// ****************************************************************************
// Shared Function Implementations
// ****************************************************************************

void OnKeyboardEvent(uint8_t *prev_buf, uint8_t *cur_buf){
    // last key released
    if (numKeysOrModsPressed(cur_buf) == 0){
        switch (CurrentMode){
            case Configuration: // send backtick on release if special function wasn't used
                PressAndReleaseKey((RichKey){ 0, _Backtick } );
                break;
            case LeftAlt: // send Left Alt on release if no other keys were pressed while it was down
                PressAndReleaseKey((RichKey){ LAlt, 0 } );
                break;
            case RightAlt: // send Right Alt on release if no other keys were pressed while it was down
                PressAndReleaseKey((RichKey){ RAlt, 0 } );
                break;
        }
        CurrentMode = NoKeys;
    }
}

// map key presses according to the current mode
RichKey MapKey(uint8_t *buf, uint8_t i){
    return KeyMaps[CurrentMode](buf, i);
}

String GetStateString(){
    String spaces = "            ";
    String stateStr = "[" + GetOSModeString() + "." + GetLayoutString() + "." + GetModeString() + "]";
    String neededSpaces = spaces.substring(0, 23 - stateStr.length());

    return stateStr + neededSpaces;
}
