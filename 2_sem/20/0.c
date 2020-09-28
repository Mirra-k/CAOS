#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>


struct ether_msg_pack {
    struct ether_header header;
    struct ether_arp arp;
};

int main(int argc, char** argv) {
    char* name = argv[1];

    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct ifreq it;
    strcpy(it.ifr_name, name);
    int ioctl_ret = ioctl(socket_fd, SIOCGIFINDEX, &it);
    if (ioctl_ret == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    int ifindex = it.ifr_ifindex;

    struct ether_msg_pack request_pack = {
        .header = {
            .ether_dhost = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
            .ether_type = htons(0x0806)
        },
        .arp = {
            .ea_hdr = {
                .ar_hrd = htons(0x0001),
                .ar_pro = htons(0x0800),
                .ar_hln = 0x0006,
                .ar_pln = 0x0004,
                .ar_op  = htons(0x0001),
            },
            .arp_spa = {0, 0, 0, 0},
            .arp_tha = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
        }
    };

    it.ifr_addr.sa_family = AF_INET;
    ioctl_ret = ioctl(socket_fd, SIOCGIFADDR, &it);
    if (ioctl_ret == -1) {
        perror("ioctl2");
        exit(EXIT_FAILURE);
    }
    memcpy(&request_pack.arp.arp_spa, it.ifr_addr.sa_data, 4);

    strcpy(it.ifr_name, name);
    ioctl_ret = ioctl(socket_fd, SIOCGIFHWADDR, &it);
    if (ioctl_ret == -1) {
        perror("ioctl3");
        exit(EXIT_FAILURE);
    }
    memcpy(request_pack.arp.arp_sha, it.ifr_hwaddr.sa_data, ETH_ALEN);
    memcpy(request_pack.header.ether_shost, it.ifr_hwaddr.sa_data, ETH_ALEN);

    char ipv4[16];
    while (scanf("%s", ipv4) != EOF) {
        struct sockaddr_in addr_in;

        int inet_ret = inet_aton(ipv4, &addr_in.sin_addr);
        if (inet_ret == 0) {
            perror("inet");
            exit(EXIT_FAILURE);
        }
        memcpy(request_pack.arp.arp_tpa, &addr_in.sin_addr.s_addr, 4);

        struct sockaddr_ll addr_ll = {
            .sll_family = AF_PACKET,
            .sll_ifindex = ifindex,
            .sll_protocol = htons(ETH_P_ARP),
            .sll_halen = ETH_ALEN
        };

        int written_bytes = sendto(socket_fd, &request_pack, sizeof(request_pack), 0,
               (struct sockaddr *)&addr_ll, sizeof(addr_ll));
        if (written_bytes == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        struct ether_msg_pack response_pack;
        while (1) {
            recv(socket_fd, &response_pack, sizeof(struct ether_msg_pack), 0);
            if (response_pack.header.ether_type == htons(0x0806) &&
                response_pack.arp.ea_hdr.ar_op == htons(0x0002) &&
                memcmp(response_pack.arp.arp_tha, it.ifr_hwaddr.sa_data,
                       sizeof(response_pack.arp.arp_tha)) == 0) {
                printf("%02x:%02x:%02x:%02x:%02x:%02x\n", response_pack.arp.arp_sha[0],
                       response_pack.arp.arp_sha[1], response_pack.arp.arp_sha[2], response_pack.arp.arp_sha[3],
                       response_pack.arp.arp_sha[4], response_pack.arp.arp_sha[5]);
                break;
            }
        }
    }

    close(socket_fd);
    return 0;
}
