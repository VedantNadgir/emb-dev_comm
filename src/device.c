#include "device.h"
#include <stdint.h>

//Internal device state
struct device_state{
    int32_t rw[6];
    int8_t control;
};
static struct device_state dev = {0};

//Device Read register function
int device_read(uint8_t reg, int32_t *out){
    if(!out){
        return DEV_ERR_VAL;
    }
    switch (reg) {
        case(REG_STATUS):
            *out = (dev.control ==0)? 1 : 0;
            return DEV_OK;
        case REG_RW0:
        case REG_RW1:
        case REG_RW2:
        case REG_RW3:
        case REG_RW4:
        case REG_RW5:
            *out = dev.rw[reg - REG_RW0];
            return DEV_OK;
        case REG_CONTROL:
            return DEV_ERR_READ;
        default:
            return DEV_ERR_BAD_REG;
    }
}

//Device Read register function
int device_write(uint8_t reg, int32_t value){
    switch (reg) {
        case(REG_STATUS):
            return DEV_ERR_WRITE;
        case REG_RW0:
        case REG_RW1:
        case REG_RW2:
        case REG_RW3:
        case REG_RW4:
        case REG_RW5:
            dev.rw[reg - REG_RW0]=value;
            return DEV_OK;
        case REG_CONTROL:
            if (value != 0 && value != 1){
                return DEV_ERR_VAL;
            }
            dev.control = value;
            return DEV_OK;
        default:
            return DEV_ERR_BAD_REG;
    }
}