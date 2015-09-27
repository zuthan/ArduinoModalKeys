#if !defined(__HELPERS_H_)
#define __HELPERS_H_

#include <Arduino.h>
#include "keys.h"

extern uint8_t NumKeysPressed(uint8_t buf[8]);
extern uint8_t NumModsPressed(uint8_t buf[8]);
extern uint8_t NumKeysOrModsPressed(uint8_t buf[8]);
extern bool IsKeyPressedInBuffer(uint8_t key, uint8_t buf[8]);
extern void OverwriteBufferWithKey(uint8_t buf[8], RichKey key);
extern void MergeKeyIntoBuffer(RichKey key, uint8_t buf[8]);
extern bool KeyIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t outbuf[8]);
extern bool ModIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t outbuf[8]);
extern void CopyBuf(uint8_t from_buf[8], uint8_t to_buf[8]);
extern void CopyKeys(uint8_t from_buf[8], uint8_t to_buf[8]);
extern bool EqualBuffers(uint8_t buf1[8], uint8_t buf2[8]);
extern bool EqualKeys(uint8_t buf1[8], uint8_t buf2[8]);

#endif // __HELPERS_H_
