


#include <cli.h>

#include <util.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp_crc32.h>
#include <csp/csp_endian.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>

#include <dirent.h>

#include <linux/rtc.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include <project.h>



void start_cli() {
    // Repeatedly take commands and execute them
    printf("Starting CLI...\n");
    printf("Starting Server...\n");

    

    // Data
    char input[1024];
    char** vect = NULL;
    int vect_len = 0;

    printf("Enter a command: (or type 'help')\n");
    // Loop
    while (1) {
        
        printf("└─ > ");
        bzero(input, 1024);
        // gets(input); // yes yes, i know
        fgets(input, 1024, stdin);
        
        char* folderpath = malloc(sizeof(char)*100);
        // Convert input to str vect
        vect = (char**)malloc(sizeof(char**) * 100); // 100 words in vect max 
        lineToVector(input, vect, &vect_len);

        // Parse for command 
        if (strcmp(vect[0], "ping") == 0) {
            int node = atoi(vect[1]);
            if(node==5){
                // Send ping
                char **set;
                char commands[1024]= "sendto 00 15 05 01 00 01 02 03 04";
                set = (char**)malloc(sizeof(char**) * 200); // 100 words in vect max 
                int set_len = 10;
                lineToVector(commands,set,&set_len);
                cli_sendto(set, set_len);
                printf("Ping Sent\n");
            }
            else{
                printf("Node %d doesn't exist. Available nodes are: '5' for ax100.\n",node);
            }
            // Removed
            // printf("Ping node %d in %dms\n", node, csp_ping(node, 1000, 20, CSP_O_CRC32));

            sleep(1);
        }else if (strcmp(vect[0], "quit") == 0) {
            printf("Quitting...\n");
            break;

        } else {
            printf("Unrecognized command %s\n", vect[0]);
        }

        

        // Free vector after command
        for (int i = 0; i < vect_len; i++) {
            free(vect[i]);
        }
        free(vect);

        // Clear input
        memset(input, 0, 1024);

        free(folderpath);
        //free(filename);
    }

    printf("Ending CLI\n");
}


uint32_t flag_byte_to_opts(uint8_t byte) {
    uint32_t opts = 0;
    if (byte & CSP_FCRC32) {opts |= CSP_O_CRC32; printf("Setting CRC option\n");}
    if (byte & CSP_FRDP) {opts |= CSP_O_RDP;}
    if (byte & CSP_FXTEA) {opts |= CSP_O_XTEA;}
    if (byte & CSP_FHMAC) {opts |= CSP_O_HMAC;}

    // Other flags are set automatically if needed

    return opts;
}

void printToConsole(csp_packet_t* packet){
    char message[256];
    memset(&message,0,256);
    FILE* fp = fopen("output1.out","a+");
    memcpy(&message,packet->data,packet->length+1);
    fprintf(fp,"OBC--> %s \n",message);
    // printf("\x1b[36m");
    // printf("OBC--> %s",message,packet->length);  
    // printf("\033[0m\n");
    fclose(fp);
}

void cli_sendto(char** vect, int len) {
    // Input sanitization
    if (len < 7) {
        printf("Not enough arguments!\n");
        return;
    }
    // Get fields from vector
    // printf("sendto: getting fields from input\n");
    uint8_t prio = atoi(vect[1]);
    uint8_t sport = atoi(vect[2]);
    uint8_t dest = atoi(vect[3]);
    uint8_t dport = atoi(vect[4]);
    // uint8_t flags = asciiToHex(vect[5]);
    uint8_t flags = 0x00;
    // printf("Flags after convertion from %s gives %02x\n",vect[5],flags);

    // Get data array
    int data_len = len - 6;
    uint8_t* data = (uint8_t*)malloc(data_len);
    for (int i = 0; i < (data_len); i++) {
        data[i] = asciiToHex(vect[6 + i]);
        // printf("sendto: converting string %s, got %02x\n", vect[6 + i], data[i]);
    }

    // Copy over to a packet
    // printf("sendto: creating packet id\n");
    csp_packet_t* packet = csp_buffer_get(256);
    packet->id.pri = prio;
    packet->id.sport = sport;
    packet->id.dst = dest;
    packet->id.dport = dport;
    packet->id.flags = flags;
    // printf("sendto: header created 0x%08x \n\n",packet->id.ext);

    // printf("sendto: creating packet data\n");
    memcpy(packet->data, data, len - 6);
    packet->length = len - 6;

    // Send it off
    // printf("sendto: sending...\n");
    csp_sendto(prio, dest, dport, sport, CSP_O_CRC32, packet, 1000);
}