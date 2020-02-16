/**
 * Definition of the default server task. It blocks on `csp_accept` while it 
 * waits for incoming connections. Upon receipt of a connection, the `conn` 
 * variable is set and you may begin reading packets with `csp_read`. 
 * 
 * If the connection or the first packet has the `FFRAG` flag set (`0x10`), then 
 * it is an SFP packet. Handle these packets with `csp_sfp_recv_fp`. 
 * 
 * Stephen Flores
 * 2019/01/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <server.h>
#include <cli.h>

#include <transport/csp_transport.h>
#include <csp/csp_crc32.h>

#include <common.h>
#include <time.h>


#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*
    Debug task that loops throughout the execution of the program
*/
CSP_DEFINE_TASK(debug_task) {

    while(1){
        // Print Remaining Buffer
        // printf("\x1b[31m--->> Remaining Buffer : %d \x1b[0m \n",csp_buffer_remaining());

        // Display Timestamp
        // variables to store date and time components
        // int hours, minutes, seconds, day, month, year;

        // time_t is arithmetic time type
        time_t now;
        
        // Obtain current time
        // time() returns the current time of the system as a time_t value
        time(&now);

        // Convert to local time format and print to stdout
        printf("\x1B[33m %s \033[0m", ctime(&now));

        csp_sleep_ms(5000);
    }

    return CSP_TASK_RETURN;
}
/**
 * Define CSP server that listens on a socket for incoming connections
 */
CSP_DEFINE_TASK(server_task) {

    // Setup socket
    csp_socket_t* socket = csp_socket(CSP_O_NONE);
    
    //printf("Binding Sockets...\n");
    
    csp_bind(socket, CSP_ANY);
    csp_listen(socket, 10);

    csp_rdp_set_opt(6,30000,16000,1,12000,3);

    if(socket== NULL){
        printf("Socket is NULL..\n");
    }

    csp_conn_t* conn;
    csp_packet_t* packet;

    while (1) {
        // Listen for incoming connections

        if ((conn = csp_accept(socket, 10000)) == NULL) {
            continue;
        }
        
        //printf("Received packet on port %d\n", csp_conn_dport(conn));
        while ((packet = csp_read(conn, 1000)) != NULL) {
            int dport = csp_conn_dport(conn);
            // int sport = csp_conn_sport(conn);
            //printf("Received packet on port %d with flags %02x\n", dport, packet->id.flags);

            /* ******************************************
                CRC VERIFICATION ADDITIONS - SARAH 2/19
                ******************************************
            */ 
            //now verify the packet to see if the method worked
            // printf("--------CRC VERIFICATION--------\n");
            // uint32_t new_head_verify = csp_htobe32(packet->id.ext);
            // printf("PG: Header FLIPPED to BE = %02x \n\n",new_head_verify);
            // packet->id.ext = new_head_verify;

            // printf("Verifying CSP Packet \n");
            // csp_crc32_verify(packet,true);

            // printf("Flipping header back to original \n");
            // packet->id.ext = csp_htobe32(packet->id.ext);
            // printf("original header = %02x \n\n", packet->id.ext);

            // printf("Packet Post-Verification: \n");
            // for (int i=0; i < packet->length; i++){
            // printf("%02x ", packet-> data[i]);
            // }
            // printf("\n\n"); 

           /*   
                END OF CRC CHECKS  
           */ 

            

            if (packet->id.flags & CSP_FFRAG) {
                csp_sfp_handler(conn, packet);
                continue;
            }

            // Handle ports here
            switch (dport) {
                case 17:
                    handleUDPFileTransfer(packet);
                    printf("Port 17 UDP File Transfer triggered...");
                    csp_buffer_free(packet);
                    break;
                case 18:
                    printToConsole(packet);
                    //printf("Port 18 UDP File Transfer triggered...");
                    csp_buffer_free(packet);
                    break;
                case 31:
                    printf("Port 31 triggered...");
                    csp_buffer_free(packet);
                    break;
                default:
                    csp_service_handler(conn, packet);
                    csp_buffer_free(packet);
                    break;
            }
        }

        csp_close(conn);
    }

    return CSP_TASK_RETURN;
}


/*
    Handle UDP File Downlink.
*/
void handleUDPFileTransfer(csp_packet_t* packet){
    printf("handleUDPFileTransfer: Saving packet as chunk...\n");
    printf("Packet Length = %d",packet->length);
    uint8_t* packet_data = malloc(packet->length+1);
    memcpy(packet_data,&packet->data,packet->length+1);
    saveChunk(packet_data,packet->length+1);
}

/**
 * Handle special case where incoming packets are for SFP
 */
void csp_sfp_handler(csp_conn_t* conn, csp_packet_t* first_packet) {
    // printf("server: Received chunk...\n");
    if (first_packet == NULL) {
        printf("server: First packet was null!\n");
    }

    // Use single, large, pre-allocated array to hold single chunk
    int max_vects = 1;
    int vect_len = 1024; 

    uint8_t* data_vects[max_vects]; // Array of pointers
    uint8_t vect0[vect_len];
    memset(vect0, 0, vect_len);
    data_vects[0] = vect0;
    int bytes_recvd = 0;

    int status = csp_sfp_recv_fp(conn, (void*)data_vects, &bytes_recvd, 1000, first_packet);
    if (status != 0) {
        printf("server: Failed to properly receive SFP stream!\n");
        return;
    }

    // Save bytes to a single allocated buffer 
    //printf("server: received chunk of size = %d bytes\n", bytes_recvd);
    uint8_t data[bytes_recvd];

    int vect_index, byte_index;
    for (vect_index = 0; vect_index < max_vects; vect_index++) { // For every vector in data_vects
        for (byte_index = 0; byte_index < vect_len; byte_index++) { 
            uint8_t byte = data_vects[vect_index][byte_index];
            data[vect_len * vect_index + byte_index] = byte;
            if (vect_len * vect_index + byte_index >= bytes_recvd) {
                break;
            }
        }
        if (vect_len * vect_index + byte_index >= bytes_recvd) {
            break;
        }
    }

    saveChunk(data, bytes_recvd);

    printf(ANSI_COLOR_RED);
    printf("server: Chunk handled\n");
    printf(ANSI_COLOR_RESET);
}


int saveChunk(uint8_t* buffer, int buffer_size) {
    printf("Saving chunk to disk...\n");

    // Parse header (print for now)
    GS_File_Transfer_Header_t header;
    int header_size = sizeof(GS_File_Transfer_Header_t);
    printf(ANSI_COLOR_GREEN);
    memcpy(&header, buffer, header_size);

    

    header.chunk_index = ( (((header.chunk_index) >> 8) & 0x00FF) | (((header.chunk_index) << 8) & 0xFF00) );
    header.total_chunks = ( (((header.total_chunks) >> 8) & 0x00FF) | (((header.total_chunks) << 8) & 0xFF00) );
    
    printf("Chunk Index: %x\nTotal Chunks: %x\n",header.chunk_index,header.total_chunks);

    printf("Server: Header is: ");
    printf("  packet_type: %d | ", header.packet_type);
    printf("  filename: %s | ", header.filename);
    printf("  chunk_index: %d | ", header.chunk_index);
    printf("  total_chunks: %d\n", header.total_chunks);
    printf(ANSI_COLOR_RESET);
    // Prepare destination file
    
    //Writing individual chunks to file
    /*
        Handle Chunk
        1. Create folder with filename
        2. Write Chunk as files 
            with name like XXXXX-YYYYY.chunk 
            where XXXXX is present chunk and,
                YYYYY is total chunks.
    */

    printf("Creating Folder...\n");

    struct stat st = {0};
    //Check if folder exists
    char foldername[50];
    memset(foldername, 0, 50);
    strncpy(foldername,header.filename,49);
    int i;
    printf("Strlen of folder = %ld\n",strlen(foldername));
    for (i=0;i<strlen(foldername);i++){
        if(foldername[i]=='/')
            foldername[i]='_';
    }
    //snprintf(foldername,13,"%s",header.filename);
    if (stat(foldername, &st) == -1) {
        mkdir(foldername, 0700);
    }

    

    //Create file with
    char filename[100];
    memset(filename, 0, 100);

    snprintf(filename, 99,"%s/%05d-%05d.chunk",foldername,header.chunk_index,header.total_chunks);
    printf("The filename is %s\n",filename);
    printf("Created file for chunk %05d\n",header.chunk_index);
    FILE* newFile = fopen(filename, "w");

    fwrite(&(buffer[header_size]), buffer_size - header_size, 1, newFile);
    
    fclose(newFile);

    // FILE* newFile = fopen("newImage.png", "a");
    // fwrite(&(data[header_size]), bytes_recvd - header_size, 1, newFile); // If header is expected, only write non-header bytes
    // // fwrite(data, bytes_recvd, 1, newFile); // If no header expected (proven)
    // fclose(newFile);

    printf("Chunk saved to disk\n");
    return 0;
}


void deChunk(char* filename){
    /*
        Write code here to do the following
        1. Call checkChunks(filename) to check chunk integrity
        2. Read all chunks in a loop and append to file.
    */
}



void handleFileCommand(csp_packet_t* packet) {
    printf("Packet bytes: ");
    for (int i = 0; i < packet->length; i++) {
        printf("%02x ", packet->data[i]);
    }
    printf("\n");

    // Handle imcoming commands
    GS_Packet_Header_t* command = (GS_Packet_Header_t*)packet->data;
    if (command->packet_type == GS_FILE_REQUEST_TYPE) {
        GS_File_Request_Header_t* request = (GS_File_Request_Header_t*)command;
        printf("Received command to retrieve file %s\n", request->filename);
        
        printf("Chunks desired: \n");
        for (int i = 0; i < request->num_chunks; i++) {
            uint16_t c = ((uint16_t*)&(request->chunk_indexes))[i];
            printf("%d ", csp_ntoh16(c));
        }
        printf("\n");
    }
}