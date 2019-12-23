
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <util.h>

#include "project.h"


short bytesToShort(const uint8_t* input, size_t index){
	assert(input!=NULL);
	index = index-1;
	short var = 0;
	var = input[index + 1] << 8 | input[index + 0];
	return var;

}

unsigned short bytesToUShort(const uint8_t* input, size_t index){
	assert(input!=NULL);
	index = index-1;
	unsigned short var = 0;
	var = input[index + 1] << 8 | input[index + 0];
	return var;

}

float bytesToFloat(const uint8_t* input, size_t index){
	assert(input!=NULL);
	index = index-1;
	union conv
	{
		uint32_t floatasInt;
		float normalFloat;
	}conv;
	
	/* Least Significant byte is first  */
	conv.floatasInt = input[index + 3] << 24 | input[index + 2] << 16 | input[index + 1] << 8 | input[index + 0];
	
	return conv.normalFloat;
}

unsigned int bytesToUint32(const uint8_t* input, size_t index){
	assert(input != NULL);
	index = index-1;
	unsigned int var = 0;
	var = input[index + 3] << 24 | input[index + 2] << 16 | input[index + 1] << 8 | input[index + 0];
	return var;
}


/* Replacement Helper functions which shifts it's reference pointer around 

	Note to anyone looking at this code:
		All of these functions shift the address pointed to by the reference pointer such that 
		the next time the reference pointer is accessed, it will be on the next set of data. This is
		so I don't have to hard code offsets which would lead to a lot of mistakes.
*/


short bytesToShort_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);

	short var = 0;
	var = input[1] << 8 | input[0];

	/* Move Pointer */
	*input_ref = *input_ref+2;
	return var;

}

unsigned short bytesToUShort_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);
	
	unsigned short var = 0;
	var = input[1] << 8 | input[0];
	/* Move Pointer */
	*input_ref = *input_ref+2;
	return var;
}

float bytesToFloat_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);

	union conv
	{
		uint32_t floatasInt;
		float normalFloat;
	}conv;
	
	/* Least Significant byte is first  */
	conv.floatasInt = input[3] << 24 | input[2] << 16 | input[1] << 8 | input[0];
	
	/* Move Pointer */
	*input_ref = *input_ref+4;
	return conv.normalFloat;
}

unsigned int bytesToUint32_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);
	unsigned int var = 0;
	var = input[3] << 24 | input[2] << 16 | input[1] << 8 | input[0];
	/* Move Pointer */
	*input_ref = *input_ref+4;
	return var;
}

/* I realize that this is pretty redundant, it serves for consistency because it still increments the reference pointer */
char bytesToChar_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);

	/* Move Pointer */
	*input_ref = *input_ref+1;
	return (char)(*input);
}

unsigned char bytesToUChar_ref(uint8_t **input_ref){
	assert(input_ref!=NULL);
	uint8_t *input = *input_ref;
	assert(input!=NULL);

	/* Move Pointer */
	*input_ref = *input_ref+1;
	return *input;
}

uint8_t asciiToHex(char* ascii) {
    // Assumed to be of length 2, if not, then bad.
    if (strlen(ascii) != 2) {
        printf("util.c: asciiToHex: string (%s) needs to be of length 2, but got %zu", ascii, strlen(ascii));
        exit(-1);
    }

    // ** Plan: take two ascii characters and form a single byte.
    uint8_t upperHalf = asciiTable(ascii[0]);
    uint8_t lowerHalf = asciiTable(ascii[1]);

    return (upperHalf << 4) + (lowerHalf);
}

uint8_t asciiTable(char aCh) {
    // If numerical
    if (48 <= aCh && aCh <= 57) {
        return (aCh & 0x0F);
    }

    // If a letter (upper or lowercase)
    if ((65 <= aCh && aCh <= 70) || (97 <= aCh && aCh <= 102)) {
        return (aCh & 0x0F) + 9;
    }

    // WARNING: DEFAULT CASE
    return 0x00;
}

void lineToVector(char* str, char** vect, int* len) {
    // vect must be initialized as: char** vect = (char**)malloc(sizeof(char**) * 10);

    // Split string into array of strings
    int word_count = 0;
    int i = 0; //current word
    int j = 0; //current letter in word i

    // Allocate first word
    vect[0] = (char*)malloc(sizeof(char*));
    // bzero(vect[0], 10);
    memset(vect[0], '\0', 10);
    for (int char_index = 0; char_index < strlen(str); char_index++) {
        if (str[char_index] == ' ') {
            // printf("Found word %s\n", vect[i]);
            i++;
            word_count++;
            j = 0;

            // Allocate next word
            vect[i] = (char*)malloc(sizeof(char*));
            // bzero(vect[i], 10);
            memset(vect[i], '\0', 10);
            continue;
        }

        if (str[char_index] == '\n') {
            // ignore darn newlines cause of fgets
            continue;
        }

        // Assign character
        // printf("Assigning character %c\n", str[char_index]);
        vect[i][j] = str[char_index];
        j++;
    }


    (*len) = word_count + 1; // account for last word

}

