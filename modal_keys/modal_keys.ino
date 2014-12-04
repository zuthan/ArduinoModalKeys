#define LEONARDO

#include <SoftwareSerial.h>
#include <USBAPI.h>
#include <hidboot.h>
#include "keymap.h"


// *******************************************************************************************
// Function Declarations
// *******************************************************************************************

void PressAndReleaseKey(RichKey key); 
void MergeKeyIntoBuffer(RichKey key, uint8_t *buf);
void CopyBuf (uint8_t *from_buf, uint8_t *to_buf);
inline void SendKeysToHost (uint8_t *buf);
void SendState(uint8_t *buf);
void PrintState(uint8_t *buf);

class KbdRptParser : public KeyboardReportParser
{
protected:
    virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};


// *******************************************************************************************
// Constants
// *******************************************************************************************


// *******************************************************************************************
// Variables
// *******************************************************************************************

bool Debug = false;

USB Usb;
HIDBoot<HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
KbdRptParser Prs;

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
    CopyBuf(outbuf, OutputBuffer);
    SendState(outbuf);
};

// *******************************************************************************************
// Helper Functions
// *******************************************************************************************

void MergeKeyIntoBuffer(RichKey key, uint8_t *buf){
    buf[0] = buf[0] | key.mods;
    for (uint8_t i=2; i<8; i++) {
        if (buf[i] == key.key) return;
        if (buf[i] == 0){
            buf[i] = key.key;
            return;
        }
    }
}

void SendState(uint8_t *buf)
{
    #ifndef LEONARDO
    if (Debug)
    #endif 
    PrintState(buf);
    
    if (!Debug)
        SendKeysToHost(buf);
}

void PrintState(uint8_t *buf)
{
    MODIFIERKEYS mod;
    *((uint8_t*)&mod) = buf[0];
    Serial.print(GetStateString());
    Serial.print("<");
    Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
    Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
    Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");
    Serial.print(".");
    Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
    Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
    Serial.print((mod.bmRightGUI    == 1) ? "G" : " ");
    Serial.print(">");
    for (uint8_t i = 2; i < 8; i++) {
        Serial.print(" ");
        uint8_t key = buf[i];
        if (key)
            PrintHex<uint8_t>(key, 0x80);
        else
            Serial.print("__");
    }
    Serial.println();
}

void CopyBuf(uint8_t *from_buf, uint8_t *to_buf){
    for (uint8_t i=0; i<8; i++) {
        to_buf[i] = from_buf[i];
    }
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
    Serial.begin( 115200 );

    if (Usb.Init() == -1 && Debug)
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
    return lhs.mods == rhs.mods && lhs.key == rhs.key;
}

void Log(String text){
    Serial.println(text);
}

void PressKey(RichKey key){
    uint8_t buf[8];
    CopyBuf(OutputBuffer, buf);
    MergeKeyIntoBuffer(key, buf);
    SendState(buf);
    delay(1);
}

void PressAndReleaseKey(RichKey key){
    PressKey(key);
    SendState(OutputBuffer);
}