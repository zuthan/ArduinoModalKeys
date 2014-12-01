#include "keymap.h"

// *******************************************************************************************
// Type Declarations
// *******************************************************************************************

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
    LeftFunc,
    LeftMod,
    RightFunc,
    RightMod,
    Tilde,
    AltTab,
} Mode;

// *******************************************************************************************
// Function Declarations
// *******************************************************************************************

RichKey DoNothing_keymap(uint8_t *buf, uint8_t i);
RichKey Normal_keymap(uint8_t *buf, uint8_t i);
RichKey FirstKeypress_keymap(uint8_t *buf, uint8_t i);
RichKey LeftFuncMode_keymap(uint8_t *buf, uint8_t i);
RichKey LeftModMode_keymap(uint8_t *buf, uint8_t i);
RichKey RightFuncMode_keymap(uint8_t *buf, uint8_t i);
RichKey RightModMode_keymap(uint8_t *buf, uint8_t i);
RichKey Tilde_keymap(uint8_t *buf, uint8_t i);
RichKey AltTab_keymap(uint8_t *buf, uint8_t i);

// *******************************************************************************************
// Constants
// *******************************************************************************************

const RichKey NoKey = { 0, 0 };

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
    &FirstKeypress_keymap, /* NoKeys */
    &Normal_keymap,        /* Normal */
    &DoNothing_keymap,     /* RemnantKeys */
    &LeftFuncMode_keymap,  /* LeftFunc */
    &LeftModMode_keymap,   /* LeftMod */
    &RightFuncMode_keymap, /* RightFunc */
    &RightModMode_keymap,  /* RightMod */
    &Tilde_keymap,         /* Tilde */
    &AltTab_keymap         /* AltTab */
};


// *******************************************************************************************
// Variables
// *******************************************************************************************

KeyboardLayout CurrentLayout = dvorak;
Mode CurrentMode = NoKeys;

// *******************************************************************************************
// Helper Functions
// *******************************************************************************************

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

RichKey AltTabKey(uint8_t *buf){
    if (numKeysPressed(buf) == 1){
        CurrentMode = AltTab;
        return (RichKey){ LAlt, _Tab };
    } else return (RichKey){ 0, _Tab };
}
bool mapModifier(uint8_t mod, Mode newMode, RichKey* out){
    *out = (RichKey){ mod, 0 };
    CurrentMode = newMode;
    return true;
}

String GetModeString(){
    switch (CurrentMode){
        case NoKeys:      return "NoKeys";
        case Normal:      return "Normal";
        case RemnantKeys: return "RemnantKeys";
        case LeftFunc:    return "LeftFunc";
        case LeftMod:     return "LeftMod";
        case RightFunc:   return "RightFunc";
        case RightMod:    return "RightMod";
        case Tilde:       return "Tilde";
        case AltTab:      return "AltTab";
    }
}
String GetLayoutString(){
    switch (CurrentLayout){
        case qwerty:    return "QY";
        case dvorak:    return "DV";
        case colemak:   return "CM";
    }
}

// *******************************************************************************************
// Mode implementations
// *******************************************************************************************

// a keymap that always does nothing
RichKey DoNothing_keymap(uint8_t *buf, uint8_t i){
    return NoKey;
}

// the keymap that just maps the key to the current keyboard layout
RichKey Normal_keymap(uint8_t *buf, uint8_t i){
    // map modifier
    if (i == 0){
        return (RichKey){ buf[0], 0 };
    }
    // map key
    else {
        uint8_t inkey = buf[i];
        switch (inkey){
            case _CapsLock:         return (RichKey){ LCtrl, 0 };
        }
        // lookup key for current keyboard layout
        if (inkey >= _A && inkey <= _CapsLock){
            uint8_t outkey = Keymap[CurrentLayout][inkey-4];
            return (RichKey){ 0, outkey };
        }

        return (RichKey){ 0, inkey };
    }
}

RichKey FirstKeypress_keymap(uint8_t *buf, uint8_t i){
    // map modifier
    if (i == 0) switch (buf[0]){
        case LAlt:
            CurrentMode = LeftFunc;
            return NoKey;
        case RAlt:
            CurrentMode = RightFunc;
            return NoKey;
    } 
    // map key
    else switch (buf[i]){
        case _Backtick:
            CurrentMode = Tilde;
            return NoKey;
    }

    CurrentMode = Normal;
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

RichKey LeftFuncMode_keymap(uint8_t *buf, uint8_t i){
    RichKey out;
    // map modifier
    if (i == 0) {
        return (RichKey){ buf[0] & ~LAlt, 0 };
    } 
    // map key
    else if (mapLAltModifier(buf[i], &out))
        return out;
    else switch (buf[i]){
        // AltTab
        case _Tab:           return AltTabKey(buf);
        case _Y:             return (RichKey){ 0, _Escape };
        case _U:             return (RichKey){ 0, _Home };
        case _I:             return (RichKey){ 0, _PgUp };
        case _O:             return (RichKey){ 0, _PgDn };
        case _P:             return (RichKey){ 0, _End };
        case _LeftBracket:   return (RichKey){ 0, _Enter };
        case _RightBracket:  return (RichKey){ 0, _Menu };
        case _Backslash:     return AltTabKey(buf);
        case _Backspace:     return (RichKey){ 0, _CapsLock };
        case _H:             return (RichKey){ 0, _Backspace };
        case _J:             return (RichKey){ 0, _Left };
        case _K:             return (RichKey){ 0, _Up };
        case _L:             return (RichKey){ 0, _Down };
        case _Semicolon:     return (RichKey){ 0, _Right };
        case _Apostrophe:    return (RichKey){ 0, _Delete };
        case _Enter:
            CurrentMode = RemnantKeys;
            return (RichKey){ LAlt,  _Enter };
        case _7:             return (RichKey){ 0, _F7 };
        case _8:             return (RichKey){ 0, _F8 };
        case _9:             return (RichKey){ 0, _F9 };
        case _0:             return (RichKey){ 0, _F10 };
        case _Dash:          return (RichKey){ 0, _F11 };
        case _Equals:        return (RichKey){ 0, _F12 };
    }
    return Normal_keymap(buf, i);
}

RichKey LeftModMode_keymap(uint8_t *buf, uint8_t i){
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
    return Normal_keymap(buf, i);
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

RichKey RightFuncMode_keymap(uint8_t *buf, uint8_t i){
    RichKey out;
    // map modifier
    if (i == 0) {
        return (RichKey){ buf[0] & ~RAlt, 0 };
    }     
    // map key
    else if (mapRAltModifier(buf[i], &out))
        return out;
    else switch (buf[i]){
        // AltTab
        case _Tab:           return AltTabKey(buf);
        case _Backslash:     return AltTabKey(buf);
        case _1:             return (RichKey){ 0, _F1 };
        case _2:             return (RichKey){ 0, _F2 };
        case _3:             return (RichKey){ 0, _F3 };
        case _4:             return (RichKey){ 0, _F4 };
        case _5:             return (RichKey){ 0, _F5 };
        case _6:             return (RichKey){ 0, _F6 };        
    }
    return Normal_keymap(buf, i);
}

RichKey RightModMode_keymap(uint8_t *buf, uint8_t i){
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
    return Normal_keymap(buf, i);
}

RichKey Tilde_keymap(uint8_t *buf, uint8_t i){
     // map modifier
    if (i >= 2) switch (buf[i]){
        case _1:
            CurrentLayout = qwerty;
            CurrentMode = RemnantKeys;
            break;
        case _2:
            CurrentLayout = dvorak;
            CurrentMode = RemnantKeys;
            break;
        case _3:
            CurrentLayout = colemak;
            CurrentMode = RemnantKeys;
            break;
    }
    return NoKey;
}

RichKey AltTab_keymap(uint8_t *buf, uint8_t i){
    // map modifier
    if (i == 0) {
        return (RichKey){ buf[0] & (LAlt | RAlt | LShift | RShift), 0 };
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

// *******************************************************************************************
// Shared Function Implementations
// *******************************************************************************************

void OnKeyboardEvent(uint8_t *prev_buf, uint8_t *cur_buf){
    // last key released
    if (numKeysOrModsPressed(cur_buf) == 0){
        switch (CurrentMode){
            case Tilde: // send backtick on release if special function wasn't used
                PressAndReleaseKey((RichKey){ 0, _Backtick} );
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
    String stateStr = "[" + GetLayoutString() + "." + GetModeString() + "]";
    String neededSpaces = spaces.substring(0, 17 - stateStr.length());
    
    return stateStr + neededSpaces;
}