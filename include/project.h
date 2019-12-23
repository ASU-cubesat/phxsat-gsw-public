#if !defined(__PROJECT_HDR__)
#define __PROJECT_HDR__

/* import libs */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* debug */
#define _VERBOSE_ 0
#if _VERBOSE_
	#define dbp(expr) printf(expr)
#else
	#define dbp(expr)
#endif

#define _INPUT_MAXLEN_ 50

/* Fn Defs from util */
float bytesToFloat(const uint8_t* input, size_t index);
unsigned int bytesToUint32(const uint8_t* input, size_t index);
unsigned short bytesToUShort(const uint8_t* input, size_t index);
short bytesToShort(const uint8_t* input, size_t index);



/* Function defs from util except they slide the pointer it's given forward by the size of their respective datatypes. 
Consequently, these should only be used with a pointer that references the data array*/
short bytesToShort_ref(uint8_t **input_ref);
unsigned short bytesToUShort_ref(uint8_t **input_ref);
float bytesToFloat_ref(uint8_t **input_ref);
unsigned int bytesToUint32_ref(uint8_t **input_ref);
/* I realize that this is pretty redundant, it serves for consistency because it still increments the reference pointer */
char bytesToChar_ref(uint8_t **input_ref);
unsigned char bytesToUChar_ref(uint8_t **input_ref);
#endif 