#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "protocol.h"
#include "device.h"

// Helper to print hex buffers for debugging
void print_packet(const char* label, const uint8_t* buf, int len) {
    printf("%s: [ ", label);
    for (int i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("]\n");
}

int main(void) {
    uint8_t resp[16];
    int resp_len = 0;
    int32_t temp_val;

    printf("--- Protocol & Device Integration Tests ---\n\n");

    // 1. TEST: VALID WRITE
    // Packet: [cmd_WRITE, REG_RW1, 0x39, 0x05, 0x00, 0x00] -> Writing 1337
    int32_t val_to_write = 1337;
    uint8_t write_req[6] = {cmd_WRITE, REG_RW1};
    memcpy(&write_req[2], &val_to_write, sizeof(val_to_write));

    printf("Action: Write 1337 to REG_RW1\n");
    if (protocol_handle_req(write_req, 6, resp, &resp_len) == 0) {
        print_packet("Response", resp, resp_len);
        // Expect: [00, 01] (Status OK, Reg 1)
    }

    // 2. TEST: VALID READ
    // Packet: [cmd_READ, REG_RW1]
    uint8_t read_req[2] = {cmd_READ, REG_RW1};
    
    printf("\nAction: Read REG_RW1\n");
    if (protocol_handle_req(read_req, 2, resp, &resp_len) == 0) {
        print_packet("Response", resp, resp_len);
        // Expect: [00, 01, 39, 05, 00, 00] (Status OK, Reg 1, Value 1337)
        memcpy(&temp_val, &resp[2], 4);
        printf("Parsed Value: %d\n", temp_val);
    }

    // 3. TEST: READ FROM WRITE-ONLY (Error Handling)
    uint8_t bad_read[2] = {cmd_READ, REG_CONTROL};
    printf("\nAction: Read REG_CONTROL (Write-Only)\n");
    protocol_handle_req(bad_read, 2, resp, &resp_len);
    print_packet("Response", resp, resp_len); 
    printf("Status Code: %d (Expected %d)\n", (int8_t)resp[0], DEV_ERR_READ);

    // 4. TEST: MALFORMED PACKET (Length mismatch)
    uint8_t short_packet[4] = {cmd_WRITE, REG_RW2, 0x01, 0x02}; // Missing 2 bytes
    printf("\nAction: Send malformed WRITE (too short)\n");
    int result = protocol_handle_req(short_packet, 4, resp, &resp_len);
    printf("Protocol Result: %d (Expected -1)\n", result);

    // 5. TEST: STATUS/CONTROL RELATIONSHIP VIA PROTOCOL
    // Write 1 to Control (Sets device to 'Busy')
    int32_t control_val = 1;
    uint8_t ctrl_req[6] = {cmd_WRITE, REG_CONTROL};
    memcpy(&ctrl_req[2], &control_val, sizeof(control_val));
    protocol_handle_req(ctrl_req, 6, resp, &resp_len);

    // Read Status
    uint8_t stat_req[2] = {cmd_READ, REG_STATUS};
    protocol_handle_req(stat_req, 2, resp, &resp_len);
    memcpy(&temp_val, &resp[2], 4);
    printf("\nAfter writing 1 to CONTROL, REG_STATUS is: %d\n", temp_val);

    printf("\n--- Tests Finished ---\n");
    return 0;
}