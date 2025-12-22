#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "device.h"

//Cmd Definition
#define cmd_READ 0x00
#define cmd_WRITE 0x01

//Response code typedef mirrors device error codes
typedef uint8_t cmd_t;
typedef uint8_t reg_t;
typedef int8_t status_t;

//Packet size
#define REQ_READ_SIZE 2 //cmd + reg
#define RES_READ_SIZE 6 //status + reg + value
#define REQ_WRITE_SIZE 6 //cmd + reg + value
#define RES_WRITE_SIZE 2

// Handles a raw request packet and fills response packet
// Returns 0 on success, -1 on malformed packet
int protocol_handle_req(const uint8_t *req, int req_len,uint8_t *resp, int *resp_len);
#endif