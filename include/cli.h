// Command-like interface for CSP


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp_crc32.h>

#ifndef CLI_H
#define CLI_H

void start_cli(void);

// Commands:
void cli_raw(char** vect, int len);
void cli_sendto(char** vect, int len);

// Helper functions:
uint32_t flag_byte_to_opts(uint8_t byte);

//Print to console
void printToConsole(csp_packet_t* packet);

#endif