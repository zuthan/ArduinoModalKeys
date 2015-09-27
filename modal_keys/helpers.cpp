#include "helpers.h"

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


bool IsKeyPressedInBuffer(uint8_t key, uint8_t buf[8]) {
    for (uint8_t i=2; i<8; i++) {
        if (buf[i] == key) return true;
    }
    return false;
}

void OverwriteBufferWithKey(uint8_t buf[8], RichKey key) {
    buf[0] = key.mods;
    buf[2] = key.key;
    for (uint8_t i=3; i<8; i++) {
        buf[i] = 0;
    }
}

void MergeKeyIntoBuffer(RichKey key, uint8_t buf[8]) {
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
bool KeyIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t outbuf[8]) {
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
bool ModIntersection(uint8_t buf1[8], uint8_t buf2[8], uint8_t outbuf[8]) {
    outbuf[0] = buf1[0] & buf2[0];
    uint8_t diff = buf1[0] & ~buf2[0];
    // Log("mods diff: " + ModifiersToString(buf1[0]) + " - " + ModifiersToString(buf2[0]) + " = " + ModifiersToString(diff));
    return !!diff;
}

void CopyBuf(uint8_t from_buf[8], uint8_t to_buf[8]) {
    for (uint8_t i=0; i<8; i++) {
        to_buf[i] = from_buf[i];
    }
}

void CopyKeys(uint8_t from_buf[8], uint8_t to_buf[8]) {
    for (uint8_t i=2; i<8; i++) {
        to_buf[i] = from_buf[i];
    }
}

bool EqualBuffers(uint8_t buf1[8], uint8_t buf2[8]) {
    bool equal = true;
    for (uint8_t i=0; i<8; i++) if (buf1[i] != buf2[i]) {
        equal = false;
    }
    return equal;
}

bool EqualKeys(uint8_t buf1[8], uint8_t buf2[8]) {
    bool equal = true;
    for (uint8_t i=2; i<8; i++) if (buf1[i] != buf2[i]) {
        equal = false;
    }
    return equal;
}
