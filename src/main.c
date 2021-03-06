
#include <stdio.h>
#include <stdlib.h>

#include <csp/csp.h>
#include <csp/interfaces/csp_if_tnc.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>

#include <server.h>
#include <common.h>
#include <config.h>
#include <cli.h>

#define PORT 31
#define MY_ADDRESS 10

//////////////////////////////////////////////////////////////////////

/*
        To disable the prompt on startup for your callsign,
        set DISABLE_CALLSIGN_PROMPT to 1 
        and, change value of callsign to your callsign
        for e.g. change "ABCDEF" to "YRSIGN"
        
        WARNING USE A VALID CALLSIGN OF LENGTH MAX 6 CHARACTERS 
        AS THIS STEP BYPASSES VALIDATION.

*/
#define DISABLE_CALLSIGN_PROMPT 0
char src_callsign[25] = "SRC";
char des_callsign[25] = "DES";

//////////////////////////////////////////////////////////////////////

#define USART_HANDLE 0

csp_thread_handle_t server_handle;
csp_thread_handle_t debug_handle;

extern uint8_t ax25_dest_src_bytes[];
extern uint8_t channel_select;

struct Configuration {
    uint8_t prompt_disable;
    char des_callsign[25];
    char src_callsign[25];
    uint8_t channel;
};

int main(int argc, char **argv) {

    // Read config file
    FILE *config;

    struct Configuration current_config;

    uint8_t custom[] = {
		0x96, 0x92, 0x6E, 0x9E, 0x9E, 0xB2, 0x60,
		0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x61
	};
    
    int i;
    for(i=0;i<14;i++){
        ax25_dest_src_bytes[i] = custom[i];
        // printf("%02x ",ax25_dest_src_bytes[i]);
    }

    bool call_ok, prompt_disable;

    config = fopen("config.phx","r");

    if(config == NULL){
        printf("Failed to open file config file.\n");
        prompt_disable = DISABLE_CALLSIGN_PROMPT; // Set this to true to bypass callsign prompt

    } else {
        if(fread(&current_config,sizeof(struct Configuration),1,config)>0){
            printf("\x1B[33mConfig File found:\n\n");
            if(current_config.prompt_disable){
                printf("Startup Prompt: Disabled\n");
            } else {
                printf("Startup Prompt: Enabled\n");
            }

            int i;
            for(i=0;i<strlen(current_config.des_callsign);i++){
                if(current_config.des_callsign[i]=='\n'){
                    des_callsign[i] = 0;
                } else {
                    des_callsign[i] = current_config.des_callsign[i];
                }
            }
            for(i=0;i<strlen(current_config.src_callsign);i++){
                if(current_config.src_callsign[i]=='\n'){
                    src_callsign[i] = 0;
                } else {
                    src_callsign[i] = current_config.src_callsign[i];
                }
            }

            channel_select = current_config.channel;

            printf("Source Callsign: %s\nDestination Callsign: %s\nChannel: 0x%02x\n\x1B[0m",src_callsign,des_callsign,current_config.channel);
            char response[20] = "";
            bool check = true, use_file = false;
            printf("Would you like to load above settings?(Y/N)\n");
            while(check){
                
                fgets(response,20,stdin);
                
                char t = response[0];
                if(t == 'Y' || t == 'y'){
                    printf("Yes!\n");
                    check = false;
                    use_file = true;
                } else if(t == 'N' || t == 'n'){
                    printf("No\n");
                    check = false;
                    use_file = false;
                }
            }
            if(use_file){
                prompt_disable = current_config.prompt_disable;
            } else {
                prompt_disable = 0;
            }
            
        }

        fclose(config);
    }

    

    call_ok = prompt_disable;

    printf("\x1B[37m");
    printf("************************************************\n");
    printf("*****    PHOENIX CUBESAT GROUND STATION    *****\n");
    printf("********** Arizona State University ************\n");
    printf("***** School of Earth & Space Exploration ******\n");
    printf("\x1B[0m");

    uint8_t invalid_char = 0;
    while(!call_ok){
        printf("\x1B[1;33m\nEnter Source CallSign:\n\x1B[0m");
        printf("\x1B[33m");
        fgets(src_callsign,1024,stdin);
        printf("\x1B[0m");
        if(strlen(src_callsign)>7){
            printf("\x1B[31mInvalid Callsign. Length greater than 6 not allowed (%ld). Try Again\n \x1B[0m",strlen(src_callsign));
            continue;
        }
        invalid_char = 0;
        for(i=0;i<strlen(src_callsign);i++){
            
            if(src_callsign[i]>='a' && src_callsign[i] <='z'){
                src_callsign[i] -= 32;
            } else if((src_callsign[i]>='0' && src_callsign[i]<='9') || (src_callsign[i]>='A' && src_callsign[i]<='Z')) {
                ;
            } else {
                invalid_char++;
            }
        }
        invalid_char--;
        if(invalid_char>0){
            printf("\x1B[31m%d Invalid character(s) detected in callsign. Try Again.\n \x1B[0m",invalid_char);
            continue;
        } 
        call_ok = true;
    }

    for(i=0;i<6;i++){
        if(i<strlen(src_callsign)){
            // printf("Setting %d th byte to %c (hex %02x:%02x)\n",i+7,callsign[i],callsign[i],callsign[i]<<1);
            if(src_callsign[i]!=0x0a){
                ax25_dest_src_bytes[i+7] = src_callsign[i]<<1;
            } else {
                ax25_dest_src_bytes[i+7] = 0;
            }
            
            // printf("%02x\n",src_callsign[i]);
        }
        else{
            ax25_dest_src_bytes[i+7] = 0;
        }
    }

    printf("\x1B[1;32mSource callsign has been set to %s\n\x1B[0m",src_callsign);
    call_ok = prompt_disable;
    // call_ok = DISABLE_CALLSIGN_PROMPT;
    invalid_char = 0;
    while(!call_ok){
        printf("\x1B[1;33m\nEnter Destination CallSign:\n\x1B[0m");
        printf("\x1B[33m");
        fgets(des_callsign,1024,stdin);
        printf("\x1B[0m");
        if(strlen(des_callsign)>7){
            printf("\x1B[31mInvalid Callsign. Length greater than 6 not allowed (%ld). Try Again\n \x1B[0m",strlen(des_callsign));
            continue;
        }
        invalid_char = 0;
        for(i=0;i<strlen(des_callsign);i++){
            
            if(des_callsign[i]>='a' && des_callsign[i] <='z'){
                des_callsign[i] -= 32;
            } else if((des_callsign[i]>='0' && des_callsign[i]<='9') || (des_callsign[i]>='A' && des_callsign[i]<='Z')) {
                ;
            } else {
                invalid_char++;
            }
        }
        invalid_char--;
        if(invalid_char>0){
            printf("\x1B[31m%d Invalid character(s) detected in callsign. Try Again.\n \x1B[0m",invalid_char);
            continue;
        } 
        call_ok = true;
    }


    for(i=0;i<6;i++){
        if(i<strlen(des_callsign)){
            // printf("Setting %d th byte to %c (hex %02x:%02x)\n",i+7,callsign[i],callsign[i],callsign[i]<<1);
            if(des_callsign[i]!=0x0a){
                ax25_dest_src_bytes[i] = des_callsign[i]<<1;
            } else {
                ax25_dest_src_bytes[i] = 0;
            }
            // printf("%02x\n",des_callsign[i]);
        }
        else{
            ax25_dest_src_bytes[i] = 0;
        }
    }

    call_ok = prompt_disable;

    char channel_tmp[25] = "";

    while(!call_ok){
        printf("\x1B[1;33m\nEnter Channel:\n\x1B[0m");
        printf("\x1B[33m");
        fgets(channel_tmp,1024,stdin);
        printf("\x1B[0m");
        if(strlen(channel_tmp)>2){
            printf("\x1B[31mInvalid Channel. Must be single digit (%ld). Try Again\n \x1B[0m",strlen(channel_tmp));
            continue;
        }
        if(channel_tmp[0]=='1'){
            channel_select = 0x10;
        } else if (channel_tmp[0]=='0'){
            channel_select = 0x00;
        } else {
            printf("Invalid channel enter either a 1 or 0\n");
            continue;
        }
        call_ok = true;
    }

    prompt_disable = 1;

    printf("\x1B[1;32mDestination callsign has been set to %s\n\x1B[0m",des_callsign);

    config = fopen("config.phx","w");

    current_config.prompt_disable = prompt_disable;
    strcpy(current_config.des_callsign,des_callsign);
    strcpy(current_config.src_callsign,src_callsign);
    current_config.channel = channel_select;

    if(fwrite(&current_config,sizeof(struct Configuration),1,config) != 0)  
        printf("contents to file written successfully !\n"); 
    else 
        printf("error writing file !\n"); 
  
    // close file 
    fclose (config); 




    

    // printf("Arguments: count = %d, args = ", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("%s ", argv[i]);
    // }
    // printf("\n");

    // Set CSP debug flags
    csp_debug_toggle_level(CSP_INFO);
    // csp_debug_toggle_level(CSP_ERROR);
    // csp_debug_toggle_level(CSP_WARN);
    // csp_debug_toggle_level(CSP_BUFFER);
    csp_debug_toggle_level(CSP_PACKET);
    csp_debug_toggle_level(CSP_PROTOCOL);
    // csp_debug_toggle_level(CSP_LOCK);

    // Initialize CSP
    csp_buffer_init(100, 300); //csp_buffer_init(100, 255 + 16);
    csp_init(MY_ADDRESS); // Use node 10 for client 

    // Initialize USART
    struct usart_conf conf;
    // conf.device = argc == 2 ? argv[1] : "/dev/ttyUSB4";
    conf.device = argc == 2 ? argv[1] : "/tmp/kisstnc";
    conf.baudrate = 9600;
    usart_init(&conf);

    // Print current TNC selected

    printf("\x1B[1;33mUsing TNC at %s\n\x1B[0m",conf.device);

    // Initialize TNC
    static csp_iface_t csp_if_tnc;
	static csp_tnc_handle_t csp_tnc_driver;
	csp_tnc_init(&csp_if_tnc, &csp_tnc_driver, usart_putc, usart_insert, "TNC");
		
    // Initialize routing
    csp_route_start_task(0, 0);
	/* Setup callback from USART RX to TNC RX */
	void my_usart_rx(uint8_t * buf, int len, void * pxTaskWoken) {
		csp_tnc_rx(&csp_if_tnc, buf, len, pxTaskWoken);
	}
	usart_set_callback(my_usart_rx);
    
    // Initialize routing
    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_tnc, CSP_NODE_MAC);
    csp_route_start_task(0, 0);

    // RDP options
    csp_rdp_set_opt(6, 30000, 16000, 1, 12000, 3);

    // //Create a RDP connection to the OBC
    // csp_conn_t* conn = csp_connect(CSP_PRIO_NORM,01,26,1000,CSP_O_RDP);
    // csp_packet_t* packet;

    // //Check if connection is made
    // if(conn==NULL){
    //     printf("Connection Failed");
    // }
    // else
    // {   
    //     //Read packets over existing connection
    //     printf("Received packet on port %d\n", csp_conn_dport(conn));
    //     while ((packet = csp_read(conn, 1000)) != NULL) {
    //         int dport = csp_conn_dport(conn);

    //         printf("Received packet on port %d with flags %02x\n", dport, packet->id.flags);

    //         if (packet->id.flags & CSP_FFRAG) {
    //             csp_sfp_handler(conn, packet);
    //             continue;
    //         }

    //         // Handle ports here
    //         switch (dport) {
    //             case 17:
    //                 //handleFileCommand(packet);
    //                 printf("Port 17 triggered...");
    //                 csp_buffer_free(packet);
    //                 break;
    //             case 31:
    //                 printf("Port 31 triggered...");
    //                 csp_buffer_free(packet);
    //                 break;
    //             default:
    //                 csp_service_handler(conn, packet);
    //                 break;
    //         }
    //     }
    // }
    

    // Start server
    csp_thread_create(server_task, "SERVER", 1000, NULL, 0, &server_handle);


    /*
        Uncomment below line to enable debugging
    */
    // csp_thread_create(debug_task, "DEBUG", 1000, NULL, 0, &debug_handle);

    // Check with printout
    //csp_conn_print_table();
    //csp_route_print_table();
    //csp_route_print_interfaces();

    // while(1){
    //     csp_sleep_ms(100000);
    // }

    // Start interactive CLI
    start_cli();
    return 0;
}