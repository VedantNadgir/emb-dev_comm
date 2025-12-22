#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
//Register definition 
enum{
    REG_RW0     = 0,
    REG_RW1     = 1,
    REG_RW2     = 2,
    REG_RW3     = 3,
    REG_RW4     = 4,
    REG_RW5     = 5,
    REG_STATUS  = 6,  // Read-only
    REG_CONTROL = 7,   // Write-only
};

//Error codes
#define DEV_OK 0
#define DEV_ERR_BAD_REG  -1
#define DEV_ERR_READ -2
#define DEV_ERR_WRITE -3
#define DEV_ERR_VAL -4

//Device API--> output is error codes
int device_read(uint8_t reg,int32_t * out); //out is pointer to the read value
int device_write(uint8_t reg,int32_t value); //value is written to the device register
#endif