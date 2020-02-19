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
                printf("SFP Handler triggered\n");
                continue;
            }

            // Handle ports here
            switch (dport) {
                case 17:
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

