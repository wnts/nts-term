#ifndef UNICODE_H_
#define UNICODE_H_
#include <stdint.h>

unsigned int unicode_codepoint_to_utf8(uint32_t codepoint, char * dst);

#endif /* UNICODE_H_ */
