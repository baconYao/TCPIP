#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <unistd.h>
#include <net/ethernet.h>

//Used to sotre reveive packet's content
char buffer[40960];

int main(int argc,char *argv[])
{
    struct sockaddr_in addr;
    struct ether_header *peth;
    struct iphdr *pip;
    struct tcphdr *ptcp;
    struct udphdr *pudp;
    
    int sock;
    int len = sizeof(addr);
    int i;
    int recv;
    char *ptemp;

    int IP_counter = 0;
    int ARP_counter = 0;
    int RARP_counter = 0;
    int TCP_counter = 0;
    int UDP_counter = 0;
    int ICMP_counter = 0;
    int IGMP_counter = 0;    

    // Create a socket
    if((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        perror("socket");
        exit(1);
    }

    struct ifreq ethreq;
    // Use NIC
    strncpy (ethreq.ifr_name, "eth0", 5);
    // Access NIC's flags
    if(ioctl(sock, SIOCGIFFLAGS,&ethreq) == -1)
    {
        perror("ioctl SIOCGIFFLAGS");
        exit(1);
    }

    //Set interface in promiscuous mode
    ethreq.ifr_flags |= IFF_PROMISC;
    if(ioctl(sock, SIOCSIFFLAGS, &ethreq) == -1)
    {
        perror("ioctl SIOCSIFFLAGS");
        exit(1);
    }

    //Use ifconfig system call
    system("ifconfig");

    // capture 100 packets
    for(i = 0; i < 100; i++)
    {
        //  return the length of the message written to the buffer
        recv = recvfrom(sock, (char *)buffer, sizeof(buffer), 0, (struct *sockaddr)&addr, &len);
    }

    printf("===============\n");
    printf("IP\t:%d\n", IP_counter);
    printf("ARP\t:%d\n", ARP_counter);
    printf("RARP\t:%d\n", RARP_counter);
    printf("TCP\t:%d\n", TCP_counter);
    printf("UDP\t:%d\n", UDP_counter);
    printf("ICMP\t:%d\n", ICMP_counter);
    printf("IGMP\t:%d\n", IGMP_counter);
    printf("===============\n");


    return 0;
}
