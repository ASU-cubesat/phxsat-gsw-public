/**
 * Provide the requisite declaration for the CSP server task. This must
 * match the result of CSP_DEFINE_TASK, so it will be platform-dependent. 
 * This definition will suit POSIX/pthread-capable systems. 
 * 
 * Stephen Flores
 * 2019/01/19
 */

#ifndef _SERVER_H
#define _SERVER_H

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp_endian.h>

#include <common.h>

// Provide define for server (must match CSP_DEFINE_TASK)
csp_thread_return_t server_task(void* param);
csp_thread_return_t debug_task(void* param);

// Chunk hanlding funcs 
void csp_sfp_handler(csp_conn_t* conn, csp_packet_t* first_packet);
int saveChunk(uint8_t* buffer, int buffer_size);
void handleFileCommand(csp_packet_t* packet);
void handleUDPFileTransfer(csp_packet_t* packet);
#endif 