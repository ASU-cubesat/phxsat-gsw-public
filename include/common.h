/**
 * Provide common definitions for things like structs that should be shared across all files
 * You can use the nifty pointer casting trick to get multiple struct types to work 
 * with a single struct. This is kinda how CFS uses pointers, by first using a generic 
 * message bus type, then casting to a specific type based on the common section. 
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <unistd.h>

// Packet types 
#define GS_FILE_TRANSFER_TYPE 0
#define GS_FILE_REQUEST_TYPE 1
#define GS_SET_SCHEDULE_TYPE 2

// Common ports 
#define SCHEDULE_INGEST_PORT 19


// Phoenix packet header 
typedef struct {
    uint8_t packet_type;
} GS_Packet_Header_t;

// Phoenix file transfer header 
typedef struct {
    uint8_t packet_type;
    char filename[19];
    uint16_t chunk_index;
    uint16_t total_chunks;
    uint32_t file_size;
} GS_File_Transfer_Header_t;

// Phoenix command header 
typedef struct {
    uint8_t packet_type;
    char filename[19];
    uint16_t num_chunks;
    uint16_t* chunk_indexes;
} GS_File_Request_Header_t;

// Phoenix write schedule header 
typedef struct {
    uint8_t packet_type;
} GS_Set_Schedule_Header_t;

typedef struct {
    uint8_t packet_type;
    uint8_t command;
    char foldername[150];    
} GS_Command_Request_t;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

#endif