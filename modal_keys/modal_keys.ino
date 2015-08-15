#define LEONARDO

#include <SoftwareSerial.h>
#include <USBAPI.h>
#include <hidboot.h>
#include "keymap.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#include <EEPROM.h>
#endif

class KbdRptParser : public KeyboardReportParser
{
protected:
    virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

// *******************************************************************************************
// Variables
// *******************************************************************************************

bool WriteToLog = true;
bool SendOutput = true;

USB Usb;
HIDBoot<HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
KbdRptParser Prs;

uint8_t InputBuffer[8] = { 0 };
uint8_t OutputBuffer[8] = { 0 };

// *******************************************************************************************
// Parse
// *******************************************************************************************

void KbdRptParser::Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    // On error - return
    if (buf[2] == 1) return;
    uint8_t outbuf[8] = { 0 };

    OnKeyboardEvent(prevState.bInfo, buf);
    for (uint8_t i=0; i<8; i++) if (i!=1 && buf[i]) {
        MergeKeyIntoBuffer(MapKey(buf, i), outbuf);
    }

    CopyBuf(buf, prevState.bInfo);
    CopyBuf(buf, InputBuffer);
    TransitionToState(outbuf);
};

// *******************************************************************************************
// Helper Functions
// *******************************************************************************************

// returns true if a new state was transmitted
bool TransitionToState(uint8_t newbuf[8]) {
    if (EqualBuffers(newbuf, OutputBuffer)) { // no need to run transition if states are already equal
        PrintState(InputBuffer, OutputBuffer, false);
        return false;
    }

    uint8_t oldbuf[8] = { 0 };
    CopyBuf(OutputBuffer, oldbuf);

    // do released keys
    uint8_t releaseKeys[8] = { 0 };
    if (KeyIntersection(oldbuf, newbuf, releaseKeys)){
        releaseKeys[0] = oldbuf[0];
        SendState(releaseKeys);
    }

    // do released mods
    uint8_t releaseMods[8] = { 0 };
    if (ModIntersection(oldbuf, newbuf, releaseMods)){
        CopyKeys(releaseKeys, releaseMods);
        SendState(releaseMods);
    }

    // do pressed mods
    uint8_t pressMods[8] = { 0 };
    if (newbuf[0] != releaseMods[0]){
        pressMods[0] = newbuf[0];
        CopyKeys(releaseKeys, pressMods);
        SendState(pressMods);
    }

    // do pressed keys
    if (!EqualKeys(newbuf, releaseKeys)){
        SendState(newbuf);
    }
    return true;
}

void SendState(uint8_t buf[8]) {
    CopyBuf(buf, OutputBuffer);
    if (WriteToLog)
        PrintState(InputBuffer, OutputBuffer, true);
    if (SendOutput){
        delay(1);
        SendKeysToHost(OutputBuffer);
    }
}

void MergeKeyIntoBuffer(RichKey key, uint8_t buf[8]){
    buf[0] = buf[0] | key.mods;
    for (uint8_t i=2; i<8; i++) {
        if (buf[i] == key.key) return;
        if (buf[i] == 0){
            buf[i] = key.key;
            return;
        }
    }
}

// Find keys present in both buf1 and buf2 and put them in outbuf.
// If there are any keys in buf1 that are not in buf2, returns true; otherwise returns false.
bool KeyIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t *outbuf) {
    bool nonEmptyDiff = false;
    for (uint8_t i=2; i<8; i++) if (buf1[i]) {
        if (IsKeyPressedInBuffer(buf1[i], buf2))
            outbuf[i] = buf1[i];
        else
            nonEmptyDiff = true;
    }
    return nonEmptyDiff;
}

// Find mods present in both buf1 and buf2 and put them in outbuf.
// If there are any mods in buf1 that are not in buf2, returns true; otherwise returns false.
bool ModIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t *outbuf) {
    outbuf[0] = buf1[0] & buf2[0];
    uint8_t diff = buf1[0] & ~buf2[0];
    // Log("mods diff: " + ModifiersToString(buf1[0]) + " - " + ModifiersToString(buf2[0]) + " = " + ModifiersToString(diff));
    return !!diff;
}

void CopyBuf(uint8_t *from_buf, uint8_t *to_buf) {
    for (uint8_t i=0; i<8; i++) {
        to_buf[i] = from_buf[i];
    }
}

void CopyKeys(uint8_t *from_buf, uint8_t *to_buf) {
    for (uint8_t i=2; i<8; i++) {
        to_buf[i] = from_buf[i];
    }
}

bool EqualBuffers(uint8_t *buf1, uint8_t *buf2) {
    bool equal = true;
    for (uint8_t i=0; i<8; i++) if (buf1[i] != buf2[i]) {
        equal = false;
    }
    return equal;
}

bool EqualKeys(uint8_t *buf1, uint8_t *buf2) {
    bool equal = true;
    for (uint8_t i=2; i<8; i++) if (buf1[i] != buf2[i]) {
        equal = false;
    }
    return equal;
}

String KeyToHexString(uint8_t key) {
  int num_nibbles = 2;
  String out = "";
  do {
          char v = 48 + (((key >> (num_nibbles - 1) * 4)) & 0x0f);
          if(v > 57) v += 7;
          out += v;
  } while(--num_nibbles);
  return out;
}

String ModifiersToString(uint8_t mods) {
    MODIFIERKEYS mod;
    *((uint8_t*)&mod) = mods;
    String str = "<" +
    String((mod.bmLeftCtrl   == 1) ? "C" : "-") +
    String((mod.bmLeftShift  == 1) ? "S" : "-") +
    String((mod.bmLeftAlt    == 1) ? "A" : "-") +
    String((mod.bmLeftGUI    == 1) ? "G" : "-") +
    "." +
    String((mod.bmRightCtrl   == 1) ? "C" : "-") +
    String((mod.bmRightShift  == 1) ? "S" : "-") +
    String((mod.bmRightAlt    == 1) ? "A" : "-") +
    String((mod.bmRightGUI    == 1) ? "G" : "-") +
    ">";
    return str;
}

String KeyToString(uint8_t key) {
    if (key) {
        return KeyToHexString(key);
    } else {
        return "__";
    }
}

String BufferToString(uint8_t buf[8]) {
    String out = "";
    out += ModifiersToString(buf[0]);
    for (uint8_t i = 2; i < 8; i++) {
        out += (" " + KeyToString(buf[i]));
    }
    return out;
}

void PrintState(uint8_t *inBuf, uint8_t *outBuf, bool outputChanged) {
    Serial.print(GetStateString());
    Serial.print(BufferToString(inBuf));
    if (outputChanged){
        Serial.print("  ==>  ");
        Serial.print(BufferToString(outBuf));
    }
    Serial.println();
}

inline void SendKeysToHost (uint8_t *buf)
{
#ifdef LEONARDO
    HID_SendReport(2,buf,8);
#else
    Serial.write(buf, 8);
#endif
}

// *******************************************************************************************
// Arduino main functions
// *******************************************************************************************

void setup()
{
    InitializeState();

    Serial.begin( 115200 );

    if (Usb.Init() == -1 && WriteToLog)
        Serial.println("OSC did not start.");

    delay( 200 );

    HidKeyboard.SetReportParser(0, (HIDReportParser*)&Prs);
}

void loop()
{
    Usb.Task();
}

// *******************************************************************************************
// Shared Function Implementations
// *******************************************************************************************
bool operator==(const RichKey& lhs, const RichKey& rhs)
{
    return lhs.mods == rhs.mods
        && lhs.key == rhs.key
        && lhs.flags == rhs.flags;
}

String RichKeyToString(RichKey key) {
    String modStr = ModifiersToString(key.mods);
    String keyStr = KeyToString(key.key);
    return modStr + keyStr;
}

void Log(String text){
    Serial.println(text);
}

void PressKey(RichKey key){
    Log("pressing Key " + RichKeyToString(key));
    uint8_t buf[8];
    CopyBuf(OutputBuffer, buf);
    MergeKeyIntoBuffer(key, buf);
    TransitionToState(buf);
}

void PressAndReleaseKey(RichKey key){
    uint8_t current_buf[8];
    CopyBuf(OutputBuffer, current_buf);

    PressKey(key);
    TransitionToState(current_buf);
}


bool IsKeyPressedInBuffer(uint8_t key, uint8_t buf[8]) {
    for (uint8_t i=2; i<8; i++) {
        if (buf[i] == key) return true;
    }
    return false;
}

