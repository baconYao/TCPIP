#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>       //for gettimeofday()
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>          //for gethostbyname()


int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in to;
    int to_len = sizeof(to);
    char buffer[65536];
    struct timeval start_time,next_time,back_time;
    int rtt=0;
    int s_time = 0, n_time = 0;
    struct icmp *icmp;
    struct ip *ip = (struct ip *)buffer;


    sock(PF_INET,SOCK_RAW,1);





    return 0;
}
