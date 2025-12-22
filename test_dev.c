#include <stdio.h>
#include "device.h"
#include <assert.h>
void test_result(const char* test_name, int result, int expected) {
    if (result == expected) {
        printf("[PASS] %s\n", test_name);
    } else {
        printf("[FAIL] %s | Expected %d, got %d\n", test_name, expected, result);
    }
}
void print_device_state(void) {
    int32_t val;
    printf("\n--- CURRENT REGISTER MAP ---\n");
    printf("REG_STATUS  [0]: ");
    if (device_read(REG_STATUS, &val) == DEV_OK) printf("%d\n", val); else printf("ERR\n");

    for (int i = REG_RW0; i <= REG_RW5; i++) {
        device_read(i, &val);
        printf("REG_RW%d     [%d]: %d\n", i - REG_RW0, i, val);
    }
    
    // Note: REG_CONTROL is write-only, so we can't read it back via device_read
    printf("REG_CONTROL [7]: WRITE-ONLY\n");
    printf("----------------------------\n\n");
}
int main(void){
    int32_t read_val = 0;
    int status;

    printf("--- Starting Device Driver Tests ---\n");

    // 1. Test Writing and Reading RW registers
    status = device_write(0, 1234);
    test_result("Write REG_RW0", status, DEV_OK);
    print_device_state();
    status = device_write(2, 34);
    test_result("Write REG_RW2", status, DEV_OK);
    print_device_state();
    status = device_read(0, &read_val);
    test_result("Read REG_RW0", status, DEV_OK);
    if (read_val == 1234) printf("Value verified: 1234\n");
    print_device_state();
    // 2. Test Write-Only Register (REG_CONTROL)
    status = device_write(REG_CONTROL, 1);
    test_result("Write Valid to REG_CONTROL", status, DEV_OK);

    status = device_read(2, &read_val);
    test_result("Read REG_RW2", status, DEV_OK);
    if (read_val == 34) printf("Value verified: 34\n");
    print_device_state();
    
    status = device_read(REG_CONTROL, &read_val);
    test_result("Read from Write-Only (REG_CONTROL)", status, DEV_ERR_READ);

    // 3. Test Read-Only Register (REG_STATUS)
    status = device_write(REG_STATUS, 50);
    test_result("Write to Read-Only (REG_STATUS)", status, DEV_ERR_WRITE);
    
    status = device_read(REG_STATUS, &read_val);
    test_result("Read REG_STATUS", status, DEV_OK);
    // Since we wrote 1 to control, status should be 0 (dev.control == 0 is false)
    printf("Status Value: %d\n", read_val);

    // 4. Test Invalid Register Address
    status = device_write(99, 10);
    test_result("Invalid Register Write", status, DEV_ERR_BAD_REG);

    // 5. Test Invalid Values for Control (Only 0 or 1 allowed)
    status = device_write(REG_CONTROL, 5);
    test_result("Invalid Value to REG_CONTROL", status, DEV_ERR_VAL);

    // 6. Test Null Pointer Safety
    status = device_read(2, NULL);
    test_result("Null Pointer Protection", status, DEV_ERR_VAL);

    printf("\n--- Tests Complete ---\n");

    return 0;
}