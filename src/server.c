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
    uint8_t buf[MAX_PKT];
    size_t buf_len = 0;

    uint8_t resp[MAX_PKT];
    int resp_len;

    while (1) {
        ssize_t n = recv(fd, buf + buf_len, sizeof(buf) - buf_len, 0);
        if (n <= 0)
            break;

        buf_len += n;

        size_t offset = 0;
        while (1) {
            if (buf_len - offset < 1)
                break;

            uint8_t cmd = buf[offset];
            int pkt_size = 0;
            if (cmd == cmd_READ) pkt_size = REQ_READ_SIZE;
            else if (cmd == cmd_WRITE) pkt_size = REQ_WRITE_SIZE;
            else {
                // Unknown byte → resync
                offset++;
                continue;
            }

            if ((int)(buf_len - offset) < pkt_size)
                break;  // Wait for more data

            if (protocol_handle_req(&buf[offset], pkt_size,
                                     resp, &resp_len) == 0) {
                send(fd, resp, resp_len, 0);
            }

            offset += pkt_size;
        }

        // Move remaining bytes to front
        if (offset > 0) {
            memmove(buf, buf + offset, buf_len - offset);
            buf_len -= offset;
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