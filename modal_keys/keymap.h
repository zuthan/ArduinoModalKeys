#if !defined(__KEYMAP_H_)
#define __KEYMAP_H_

#include <Arduino.h>

extern void InitializeState();
extern void TransformBuffer(uint8_t buf[8], uint8_t outbuf[8]);
extern String GetStateString();

#endif // __KEYMAP_H_
