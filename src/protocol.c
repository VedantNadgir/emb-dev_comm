#include "protocol.h"
#include <stdint.h>
#include <string.h>

int protocol_handle_req(const uint8_t *req, int req_len, uint8_t *resp, int *resp_len){
    if(!req || !resp || !resp_len){
        return -1;
    }
    cmd_t cmd = req[0];
    reg_t reg = req[1];
    int32_t value;
    status_t status;

    if(cmd == cmd_READ){
        //READ success → 6 bytes
        //READ error → 2 bytes
        if(req_len != REQ_READ_SIZE){
            return -1;
        }
        status = device_read(reg, &value);
        resp[0] = (uint8_t)status;
        resp[1] = reg;
        if(status == DEV_OK){
            memcpy(&resp[2], &value, sizeof(value));
            *resp_len = RES_READ_SIZE;
        }else {
            *resp_len = 2; // Set to 2 so the caller ignores the rest of the buffer
        }
        return 0;
    }else if(cmd == cmd_WRITE){  //cmd == cmd_READ
        if(req_len != REQ_WRITE_SIZE){
            return -1;
        }
        memcpy(&value, &req[2], sizeof(value));
        status = device_write(reg, value);
        resp[0] = (uint8_t)status;
        resp[1] = reg;
        *resp_len = RES_WRITE_SIZE;
        return 0;
    }
    return -1;
}