#include "server.h"
#include <stdio.h>

int main() {
    int port = 8080;
    // Note: You might need to add code to handle SIGINT (Ctrl+C) 
    // to close the socket properly.
    if (server_run(port) < 0) {
        fprintf(stderr, "Server failed to start.\n");
        return 1;
    }
    return 0;
}