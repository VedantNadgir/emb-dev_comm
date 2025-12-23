#include "server.h"
#include "protocol.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PKT 64

static void handle_client(int fd) {
    uint8_t req[MAX_PKT];
    uint8_t resp[MAX_PKT];
    int resp_len;

    ssize_t n;
    while (1) {
        n = recv(fd, req, sizeof(req), 0);
        if (n <= 0) {
            // Client closed connection or error
            break;
        }

        int offset = 0;
        while (offset < n) {
            uint8_t cmd = req[offset];

            int pkt_size = 0;
            if (cmd == cmd_READ) pkt_size = REQ_READ_SIZE;
            else if (cmd == cmd_WRITE) pkt_size = REQ_WRITE_SIZE;
            else {
                // Unknown command → skip byte
                offset++;
                continue;
            }

            // Ensure we have full packet
            if (offset + pkt_size > n) {
                // Malformed / truncated packet → protocol error
                uint8_t err_resp[2];
                err_resp[0] = DEV_ERR_VAL;  // or a dedicated PROTO_ERR_TRUNCATED
                err_resp[1] = 0xFF;
                send(fd, err_resp, 2, 0);
                return;  // Close client connection
            }

            // Process one packet
            if (protocol_handle_req(&req[offset], pkt_size, resp, &resp_len) == 0) {
                send(fd, resp, resp_len, 0);
            }
            offset += pkt_size; // Move to next packet in buffer
        }
    }
}


int server_run(int port){
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, 5) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
        close(client_fd);
    }
}