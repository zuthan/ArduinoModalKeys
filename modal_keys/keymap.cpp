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
    LeftFunc,
    LeftMod,
    RightAlt,
    RightFunc,
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

// ****************************************************************************
// Function Declarations
// ****************************************************************************

// helpers
void loadOSMode();
void setOSMode(OSMode osMode);
void setMode(Mode mode, uint8_t *buf);
RichKey enterMode(Mode mode, uint8_t *buf, uint8_t i);
bool numKeysPressed(uint8_t *buf);
bool numModsPressed(uint8_t *buf);
bool numKeysOrModsPressed(uint8_t *buf);
RichKey AltTabKey(uint8_t *buf, Side side);
bool mapModifier(uint8_t mod, Mode newMode, RichKey* out);
RichKey sendKey(uint8_t keycode);
RichKey sendModifiers(uint8_t mods);
RichKey sendKeyCombo(uint8_t mods, uint8_t keycode);
String GetOSModeString();
String GetModeString();
String GetLayoutString();

// pure functions
RichKey mapLeftFuncKey(uint8_t *buf, uint8_t i);
RichKey mapRightFuncKey(uint8_t *buf, uint8_t i);

// state changing methods
RichKey Configuration_keymap(uint8_t *buf, uint8_t i);

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

RichKey GamingEntryPoint_keymap(uint8_t *buf, uint8_t i);
RichKey GamingBacktick_keymap(uint8_t *buf, uint8_t i);
RichKey GamingTab_keymap(uint8_t *buf, uint8_t i);
RichKey GamingCapsLock_keymap(uint8_t *buf, uint8_t i);
RichKey GamingSpace_keymap(uint8_t *buf, uint8_t i);

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
    &LeftFuncMode_keymap,       /* LeftFunc */
    &LeftModMode_keymap,        /* LeftMod */
    &RightAltMode_keymap,       /* RightAlt */
    &RightFuncMode_keymap,      /* RightFunc */
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
bool ModeIsDirty = false;

// ****************************************************************************
// State Dependant Values
// ****************************************************************************

RichKey CapsLockKey() {
    switch(CurrentOSMode){
        case Windows: return sendModifiers(LCtrl);
        case OSX: return sendModifiers(LGui);
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

RichKey Configuration_keymap(uint8_t *buf, uint8_t i) {
     // map modifier
    if (i == 0) switch (buf[0]) {
        case LCtrl:
        case RCtrl:
            setOSMode(Windows);
            break;
        case LGui:
        case RGui:
            setOSMode(OSX);
            break;
    }

     // map key
    if (i >= 2) switch (buf[i]){
        case _Escape: return NoKey;
        case _F1:
            CurrentLayout = qwerty;
            EntryPointMode = NoKeys;
            break;
        case _F2:
            CurrentLayout = dvorak;
            EntryPointMode = NoKeys;
            break;
        case _F3:
            CurrentLayout = qwerty;
            EntryPointMode = GamingNoKeys;
            break;
    }

    return NoKey;
}

RichKey EntryPoint_keymap(uint8_t *buf, uint8_t i) {
    // Modifier entry points
    switch (buf[0]){
        case LAlt: return enterMode(LeftAlt, buf, i);
        case RAlt: return enterMode(RightAlt, buf, i);
    }

    // normal key entry points
    switch (buf[2]){
        case _Escape:   return enterMode(Configuration, buf, i);
    }

    // No special behavior activated. Apply normal keyboard behavior.
    return enterMode(Normal, buf, i);
}

// a keymap that always does nothing
RichKey DoNothing_keymap(uint8_t *buf, uint8_t i){
    return NoKey;
}

RichKey mapNormalKeyToCurrentLayout(uint8_t *buf, uint8_t i) {
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
            return sendKey(outkey);
        }

        return sendKey(inkey);
    }
}

// the keymap that just maps the key to the current keyboard layout
RichKey Normal_keymap(uint8_t *buf, uint8_t i) {
    return mapNormalKeyToCurrentLayout(buf, i);
}

bool mapLAltModifier(uint8_t key, RichKey* out) {
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

RichKey LeftAltMode_keymap(uint8_t *buf, uint8_t i) {
    RichKey funcKey = mapLeftFuncKey(buf, 2);
    // map modifier
    if (i == 0) switch (buf[i]) {
        case LAlt:
            if (numKeysPressed(buf) == 0)
                return NoKey; // don't press LAlt by default - use it as a custom modifier
            else if (funcKey == NoKey){ // a non-func-mode key was pressed first
                // press Alt key slightly earlier than other keys
                PressKey((RichKey){ LAlt, 0 });
            }
    }
    // map secondary modifier
    else if (i == 2) switch (buf[i]) {
        case _X:           return enterMode(NumPad, buf, i);
        case _C:           return enterMode(WindowSnap, buf, i);
    }

    // map key
    if (funcKey == NoKey){ // a non-func-mode key was pressed first
        // Switch to Normal keyboard where Alt key behaves like Alt key
        return enterMode(Normal, buf, i);
    }
    else {
        return enterMode(LeftFunc, buf, i);
    }
}

RichKey mapLeftFuncKey(uint8_t *buf, uint8_t i) {
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
        // --- Left Hand ---
        case _Tab:           return AltTabKey(buf, Left);
        case _1:             return sendKey(_F1);
        case _2:             return sendKey(_F2);
        case _3:             return sendKey(_F3);
        case _4:             return sendKey(_F4);
        case _5:             return sendKey(_F5);
        case _6:             return sendKey(_F6);

        case _X:             return enterMode(NumPad, buf, i);
        case _C:             return enterMode(WindowSnap, buf, i);

        // --- Right Hand ---
        case _Y:             return sendKey(_Escape);
        case _U:             return sendKey(_Home);
        case _I:             return sendKey(_PgUp);
        case _O:             return sendKey(_PgDn);
        case _P:             return sendKey(_End);
        case _LeftBracket:   return sendKey(_Enter);
        case _RightBracket:  return sendKey(_Menu);
        case _Backslash:     return AltTabKey(buf, Left);
        case _Backspace:     return sendKey(_CapsLock);
        case _H:             return sendKey(_Backspace);
        case _J:             return sendKey(_Left);
        case _K:             return sendKey(_Up);
        case _L:             return sendKey(_Down);
        case _Semicolon:     return sendKey(_Right);
        case _Apostrophe:    return sendKey(_Delete);
        case _7:             return sendKey(_F7);
        case _8:             return sendKey(_F8);
        case _9:             return sendKey(_F9);
        case _0:             return sendKey(_F10);
        case _Dash:          return sendKey(_F11);
        case _Equals:        return sendKey(_F12);
    }
    return NoKey;
}

RichKey LeftFuncMode_keymap(uint8_t *buf, uint8_t i) {
    return mapLeftFuncKey(buf, i);
}

RichKey LeftModMode_keymap(uint8_t *buf, uint8_t i) {
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

bool mapRAltModifier(uint8_t key, RichKey* out) {
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

RichKey RightAltMode_keymap(uint8_t *buf, uint8_t i) {
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
        return enterMode(Normal, buf, i);
    else
        return enterMode(RightFunc, buf, i);
}

RichKey mapRightFuncKey(uint8_t *buf, uint8_t i) {
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
        case _1:             return sendKey(_F1);
        case _2:             return sendKey(_F2);
        case _3:             return sendKey(_F3);
        case _4:             return sendKey(_F4);
        case _5:             return sendKey(_F5);
        case _6:             return sendKey(_F6);
    }
    return NoKey;
}

RichKey RightFuncMode_keymap(uint8_t *buf, uint8_t i) {
    return mapRightFuncKey(buf, i);
}

RichKey RightModMode_keymap(uint8_t *buf, uint8_t i) {
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

RichKey AltTab_keymap(uint8_t *buf, uint8_t i) {
    // map modifier
    if (i == 0) {
        uint8_t Mods = 0;
        if (buf[0] & LAlt) Mods |= AppSwitchModifierKeycode(Left);
        if (buf[0] & RAlt) Mods |= AppSwitchModifierKeycode(Right);
        if (buf[0] & LShift) Mods |= LShift;
        if (buf[0] & RShift) Mods |= RShift;
        return sendModifiers(Mods);
    }
    // map key
    else switch (buf[i]){
        // AltTab
        case _Tab:
        case _Backslash:
            return sendKey(_Tab);
        case _Q:
            return sendModifiers(LShift);
        case _P:
            return sendModifiers(RShift);
    }
    return NoKey;
}

RichKey WindowSnap_keymap(uint8_t *buf, uint8_t i) {
    // exit condition: first key pressed is no longer _C
    if (buf[2] != _C) return enterMode(NoKeys, buf, i);

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

RichKey NumPad_keymap(uint8_t *buf, uint8_t i) {
    // exit condition: first key pressed is no longer _X
    if (buf[2] != _X) return enterMode(NoKeys, buf, i);

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
        case _7:                    return sendKey(_Numpad7);
        case _8:                    return sendKey(_Numpad8);
        case _9:                    return sendKey(_Numpad9);
        case _0:                    return sendKey(_NumpadTimes);
        case _U:                    return sendKey(_Numpad4);
        case _I:                    return sendKey(_Numpad5);
        case _O:                    return sendKey(_Numpad6);
        case _P:                    return sendKey(_NumpadMinus);
        case _LeftBracket:          return sendKey(_NumpadEnter);
        case _RightBracket:         return sendKey(_NumLock);
        case _H:                    return sendKey(_Backspace);
        case _J:                    return sendKey(_Numpad1);
        case _K:                    return sendKey(_Numpad2);
        case _L:                    return sendKey(_Numpad3);
        case _Semicolon:            return sendKey(_NumpadPlus);
        case _Apostrophe:           return sendKeyCombo(LShift, _Dash); // Underscore
        case _N:                    return sendKeyCombo(LShift, _Semicolon); // Colon
        case _M:                    return sendKey(_Numpad0);
        case _Comma:                return sendKey(_Comma);
        case _Fullstop:             return sendKey(_Fullstop);
        case _ForwardSlash:         return sendKey(_NumpadDivide);
        case _Space:                return sendKey(_Space);
        case _Enter:                return sendKey(_Enter);
    }
    return NoKey;
}

// ================== Gaming Mode ===================

RichKey GamingEntryPoint_keymap(uint8_t *buf, uint8_t i) {
    // map modifier
    if (i == 0) switch (buf[0]) {
        case LGui:        return sendKey(_Enter);
    }
    // map key
    else if (i == 2) switch (buf[i]) {
        case _Escape:     return enterMode(Configuration, buf, i);
        case _Backtick:   return enterMode(GamingBacktick, buf, i);
        case _Tab:        return enterMode(GamingTab, buf, i);
        case _CapsLock:   return enterMode(GamingCapsLock, buf, i);
        case _Space:      return enterMode(GamingSpace, buf, i);
    }

    // No special behavior activated. Apply normal keyboard behavior.
    return mapNormalKeyToCurrentLayout(buf, i);
}


RichKey GamingBacktick_keymap(uint8_t *buf, uint8_t i) {
    // map modifier
    if (i == 0) switch (buf[0]) {

    }
    // map key
    else switch (buf[i]) {
        case _Backtick:      return NoKey;
        case _Space:         return sendModifiers(LCtrl);
        // backtick + row0 number ==> LH function key
        case _1:             return sendKey(_F1);
        case _2:             return sendKey(_F2);
        case _3:             return sendKey(_F3);
        case _4:             return sendKey(_F4);
        case _5:             return sendKey(_F5);
        case _6:             return sendKey(_F6);
    }
    return NoKey;
}

RichKey GamingTab_keymap(uint8_t *buf, uint8_t i) {
     // map modifier
    if (i == 0) switch (buf[0]) {

    }
    // map key
    else switch (buf[i]){
        case _Tab:           return NoKey;
        case _Space:         return sendModifiers(LCtrl);
        // Tab + row0 number ==> shift + LH number
        case _1:             return sendKeyCombo(LShift, _1);
        case _2:             return sendKeyCombo(LShift, _2);
        case _3:             return sendKeyCombo(LShift, _3);
        case _4:             return sendKeyCombo(LShift, _4);
        case _5:             return sendKeyCombo(LShift, _5);
        case _6:             return sendKeyCombo(LShift, _6);
        // Tab + row1 letter ==> shift + RH number
        case _Q:             return sendKeyCombo(LShift, _6);
        case _W:             return sendKeyCombo(LShift, _7);
        case _E:             return sendKeyCombo(LShift, _8);
        case _R:             return sendKeyCombo(LShift, _9);
        case _T:             return sendKeyCombo(LShift, _0);
    }
    return mapNormalKeyToCurrentLayout(buf, i);
}


RichKey GamingCapsLock_keymap(uint8_t *buf, uint8_t i) {
     // map modifier
    if (i == 0) switch (buf[0]) {

    }
    // map key
    else switch (buf[i]){
        case _CapsLock:      return NoKey;
        case _Space:         return sendModifiers(LCtrl);
        // CapsLock + row1 letter ==> RH number
        case _Q:             return sendKey(_6);
        case _W:             return sendKey(_7);
        case _E:             return sendKey(_8);
        case _R:             return sendKey(_9);
        case _T:             return sendKey(_0);
    }
    return mapNormalKeyToCurrentLayout(buf, i);
}

RichKey GamingSpace_keymap(uint8_t *buf, uint8_t i) {
    // map modifier
    if (i == 0) switch (buf[0]) {
        // Space + Shift ==> RH function key (F7)
        case LShift:       return sendKey(_F7);
        case LAlt:         return sendKey(_Space);
    }
    // map key
    else switch (buf[i]){
        case _Space:         return NoKey;
        case _Backtick:      return enterMode(GamingBacktick, buf, i);
        case _Tab:           return enterMode(GamingTab, buf, i);
        case _CapsLock:      return enterMode(GamingCapsLock, buf, i);
         // Space + row0 number ==> ctrl + LH number
        case _1:             return sendKeyCombo(LCtrl, _1);
        case _2:             return sendKeyCombo(LCtrl, _2);
        case _3:             return sendKeyCombo(LCtrl, _3);
        case _4:             return sendKeyCombo(LCtrl, _4);
        case _5:             return sendKeyCombo(LCtrl, _5);
        case _6:             return sendKeyCombo(LCtrl, _6);
        // Space + R1,R2 letter keys ==> navigation keys
        case _Q:             return sendKey(_Delete);
        case _W:             return sendKey(_PgUp);
        case _E:             return sendKey(_Up);
        case _R:             return sendKey(_PgDn);
        case _T:             return sendKey(_Backspace);
        case _A:             return sendKey(_Home);
        case _S:             return sendKey(_Left);
        case _D:             return sendKey(_Down);
        case _F:             return sendKey(_Right);
        case _G:             return sendKey(_End);
        // Space + R3 letter keys ==> RH function keys
        case _Z:             return sendKey(_F8);
        case _X:             return sendKey(_F9);
        case _C:             return sendKey(_F10);
        case _V:             return sendKey(_F11);
        case _B:             return sendKey(_F12);

    }
    return mapNormalKeyToCurrentLayout(buf, i);
}


// ****************************************************************************
// Helper Functions
// ****************************************************************************

void loadOSMode() {
    OSMode osMode = Windows;
    EEPROM.get( OSModeSlot, osMode );
    CurrentOSMode = osMode;
}

void setOSMode(OSMode osMode) {
    CurrentOSMode = osMode;
    EEPROM.put( OSModeSlot, osMode );
}

void setMode(Mode mode, uint8_t *buf) {
    CurrentMode = mode;
    ModeIsDirty = (numKeysOrModsPressed(buf) > 1);
}

RichKey enterMode(Mode mode, uint8_t *buf, uint8_t i) {
    setMode(mode, buf);
    return MapKey(buf, i);
}

bool numKeysPressed(uint8_t *buf) {
    uint8_t count = 0;
    for (uint8_t i=2; i<8; i++) {
        if (buf[i]) count++;
    }
    return count;
}

bool numModsPressed(uint8_t *buf) {
    uint8_t bitset = buf[0];
    uint8_t count;
    for (count = 0; bitset; count++)
        bitset &= bitset - 1;
    return count;
}

bool numKeysOrModsPressed(uint8_t *buf) {
    return numModsPressed(buf) + numKeysPressed(buf);
}

RichKey AltTabKey(uint8_t *buf, Side side) {
    if (numKeysPressed(buf) == 1){
        CurrentMode = AltTab;
        return (RichKey){ AppSwitchModifierKeycode(side), _Tab };
    } else return sendKey(_Tab);
}

bool mapModifier(uint8_t mod, Mode newMode, RichKey* out) {
    *out = (RichKey){ mod, 0 };
    CurrentMode = newMode;
    return true;
}

RichKey sendKey(uint8_t keycode) {
    return sendKeyCombo(0, keycode);
}

RichKey sendModifiers(uint8_t mods) {
    return sendKeyCombo(mods, 0);
}

RichKey sendKeyCombo(uint8_t mods, uint8_t keycode) {
    ModeIsDirty = true;
    return (RichKey){ mods, keycode };
}


String GetOSModeString() {
    switch (CurrentOSMode){
        case Windows:    return "Win";
        case OSX:        return "OSX";
    }
}

String GetModeString() {
    switch (CurrentMode){
        case Configuration:   return "Configuration";
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
        case GamingNoKeys:    return "GamingNoKeys";
        case GamingBacktick:  return "GamingBacktick";
        case GamingTab:       return "GamingTab";
        case GamingCapsLock:  return "GamingCapsLock";
        case GamingSpace:     return "GamingSpace";
        default:              return "<unknown>";
    }
}

String GetLayoutString() {
    switch (CurrentLayout){
        case qwerty:    return "QY";
        case dvorak:    return "DV";
        case colemak:   return "CM";
    }
}

String GetDirtyString() {
    return ModeIsDirty ? "*" : "";
}

// ****************************************************************************
// Shared Function Implementations
// ****************************************************************************

void InitializeState() {
    loadOSMode();
}

void OnKeyboardEvent(uint8_t *prev_buf, uint8_t *cur_buf) {
    // last key released
    if (numKeysOrModsPressed(cur_buf) == 0) {
        // send normal keys on release of custom modifier if no other keys were pressed while it was held down
        if (ModeIsDirty == false) switch (CurrentMode) {
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
        setMode(EntryPointMode, cur_buf);
        return;
    }
}

// map key presses according to the current mode
RichKey MapKey(uint8_t *buf, uint8_t i) {
    return KeyMaps[CurrentMode](buf, i);
}

String GetStateString(){
    String spaces = "                              ";
    String stateStr = "[" + GetOSModeString() + "." + GetLayoutString() + "." + GetModeString() + GetDirtyString() + "]";
    String neededSpaces = spaces.substring(0, 26 - stateStr.length());

    return stateStr + neededSpaces;
}
