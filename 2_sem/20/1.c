#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <linux/limits.h>
#include <assert.h>

struct some_header {
    unsigned short query_id;
	unsigned short flags;
	unsigned short QDCOUNT;
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;
} __attribute__((__packed__));

struct some_tailer {
    unsigned short qtype;
    unsigned short qclass;
} __attribute__((__packed__));

 void domen2q(char* str, unsigned char* answer) {
    char* token = strtok(str, ".");

    int indent = 0;
    while (token != NULL)
    {
        uint8_t token_size = strlen(token);
        memcpy(answer + indent, &token_size, 1);
        memcpy(answer + indent + 1, token, token_size);

        indent += token_size + 1;
        token = strtok(NULL, "."); 
    }
    memset(answer + indent, 0x00, 1);
    //printf("%s\n", answer);
    //return answer;
}

int main() {
    struct some_header header = {
        .query_id = htons(getpid() & 0xFFFF),
        .flags = htons(0x0100),
        .QDCOUNT = htons(0x0001),
        .ANCOUNT = htons(0x0000),
        .NSCOUNT = htons(0x0000),
        .ARCOUNT = htons(0x0000)
    };
    size_t header_size = sizeof(header);
    struct some_tailer tailer = {
        .qtype = htons(0x0001),
        .qclass = htons(0x0001)
    };
    size_t tailer_size = sizeof(tailer);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert (socket_fd != -1);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("8.8.8.8"),
        .sin_port = htons(53)
    };

    char buffer[100];
    while (scanf("%s", buffer) != EOF) {
        int name_size = strlen(buffer) + 2;
        unsigned char name[name_size];
        domen2q(buffer, name);

        int indent = 0;
        int query_size = name_size + header_size + tailer_size;
        unsigned char query[query_size];

        memcpy(query, (unsigned char*)&header, header_size);
        indent += header_size;
        memcpy(query + indent, name, name_size);
        indent += name_size;
        memcpy(query + indent, (unsigned char*)&tailer, tailer_size);

        //printf("%s %ld\n", query, strlen(query));

        sendto(socket_fd, &query, query_size, 0, (struct sockaddr*)&addr, sizeof(addr));

        unsigned char answer[PATH_MAX];
        size_t recv_size = recv(socket_fd, answer, sizeof(answer), 0);
        assert(recv_size != -1);

        for (size_t i = recv_size - 4; i < recv_size - 1; ++i) {
            printf("%d.", answer[i]);
        }
        printf("%d\n", answer[recv_size - 1]);
    }

    close(socket_fd);
    return 0;
}
