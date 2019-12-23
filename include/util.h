

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef UTIL_H
#define UTIL_H

// Convert two-character ascii string to a hex value
uint8_t asciiToHex(char* ascii);
uint8_t asciiTable(char aCh);

// Convert line of text to vector of strings
void lineToVector(char* str, char** vect, int* len);


#endif