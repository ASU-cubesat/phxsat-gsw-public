


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
            printf("Ping node %d in %dms\n", node, csp_ping(node, 1000, 20, CSP_O_CRC32));

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