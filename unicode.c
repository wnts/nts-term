#include "unicode.h"

#include <stdint.h>

unsigned int unicode_codepoint_to_utf8(uint32_t codepoint, char *dst)
{
    if(codepoint <= 0x007F)
    {
        dst[0] = codepoint;
        return 1;
    }
    else if(codepoint <= 0x07FF)
    {
        dst[0] = (codepoint >> 6) | 0xC0;
        dst[1] = (codepoint & 0x3F) | 0x80;
        return 2;
    }
    else if(codepoint <= 0xFFFF)
    {
        dst[0] = (codepoint >> 12) | 0xE0;
        dst[1] = ((codepoint >> 6) & 0x3F) | 0x80;
        dst[2] = (codepoint & 0x3F) | 0x80;
        return 3;
    }
    else if(codepoint < 0x10FFFF)
    {
        dst[0] = (codepoint >> 18) | 0xF0;
        dst[1] = ((codepoint >> 12) & 0x3F) | 0x80;
        dst[2] = ((codepoint >> 6) & 0x3F) | 0x80;
        dst[3] = (codepoint & 0x3F) | 0x80;
        return 4;
    }
    else
    {
        return 0;
    }
}
