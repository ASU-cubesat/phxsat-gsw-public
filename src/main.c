
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

#define USART_HANDLE 0

csp_thread_handle_t server_handle;
csp_thread_handle_t debug_handle;

int main(int argc, char **argv) {
    printf("\x1B[37m");
    printf("************************************************\n");
    printf("*****    PHOENIX CUBESAT GROUND STATION    *****\n");
    printf("********** Arizona State University ************\n");
    printf("***** School of Earth & Space Exploration ******\n");
    printf("*************** Public Release *****************\n");
    printf("************************************************\n");
    printf("\x1B[0m");


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
    // csp_debug_toggle_level(CSP_PACKET);
    // csp_debug_toggle_level(CSP_PROTOCOL);
    // csp_debug_toggle_level(CSP_LOCK);

    // Initialize CSP
    csp_buffer_init(100, 300); //csp_buffer_init(100, 255 + 16);
    csp_init(MY_ADDRESS); // Use node 10 for client 

    // Initialize USART
    struct usart_conf conf;
    conf.device = argc == 2 ? argv[1] : "/dev/ttyUSB0";
    conf.baudrate = 9600;
    usart_init(&conf);

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
        Uncomment below line to monitor remaining buffers
    */
    //csp_thread_create(debug_task, "DEBUG", 1000, NULL, 0, &debug_handle);

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