#if !defined(__MODAL_KEYS_H_)
#define __MODAL_KEYS_H_

#include "keys.h"

extern String RichKeyToString(RichKey key);
extern String BufferToString(uint8_t buf[8]);
extern void Log(String text);
extern void PressAndReleaseKey(RichKey key);

#endif // __MODAL_KEYS_H_
