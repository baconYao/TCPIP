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

int getIp(void);        //取得目的端的IP address
unsigned short cksum(unsigned short *, int);    //計算CHECKSUM
int rtt_c(int,int,int,int);     //計算ROUND TRIP TIME

char *hostip;
struct sockaddr_in to;
int to_len = sizeof(to);

int main(int argc, char *argv[])
{
    int sock;
    int seqnum = 0;           //SEQ Number從0開始
    char buffer[65536];
    struct timeval start_time,next_time,back_time;
    int rtt=0;
    int s_time = 0, n_time = 0;
    struct icmp *icmp;
    struct ip *ip = (struct ip *)buffer;

    //Use ICMP packet
    sock(PF_INET,SOCK_RAW,1);
    if(sock == -1)
    {
        perror("Socket can't creat!\n");
        exit(1);
    }

    // 取得Address
    if(getIp() == 0)
    {
        exit(1);
    }

    printf("Pining %s\n\n", argv[1]);
    //將Buffer初始化
    bzero(buffer,buffersize + 16);
    //轉為ICMP格式
    icmp = (struct icmp *)buffer;
    



    

    return 0;
}


int getIp()
{
    char *hostname;
    struct in_addr taraddr;
    struct hostent *hp;
    int j;


    hp = gethostbyname(argv[1]);           //查詢hostname的地址
    if(hp) {
       hostname = hp->h_name;
       to.sin_family = hp->h_addrtype;
       bcopy(hp->h_addr,(caddr_t)&to.sin_addr,hp->h_length);
       for(j = 0; hp->h_addr_list[j]; j++) {
          memcpy(&taraddr.s_addr, hp->h_addr_list[j], hp->h_length);
          hostip = inet_ntoa(taraddr);   
       }
       return 1;
    }
    else {                                 //DESTINATION輸入錯誤
       printf("ping: unknown host %s\n", argv[1]);
       return 0;
    }


}


//計算ICMP封包的CHECKSUM
unsigned short cksum(unsigned short *section, int icmp_byte)
{
   int sum = 0;
   unsigned short *sec;
   unsigned short temp;
   unsigned short answer = 0;

   sec = section;            //每16bits為一個Section來計算
 
   while(icmp_byte > 1) {
      sum += *sec++;
      icmp_byte -= 2;
   }
   if(icmp_byte == 1) {      //最後一個Section只有一個byte
      *(unsigned char *)&temp = *(unsigned char *)sec;
      sum += temp;
   }

   sum = (sum >> 16) + (sum & 0xffff);      //將進位加回Partial SUM
   sum += (sum >> 16);                      //可能又產生進位
   answer = ~sum;                           //取Complement

   return answer;
}

//計算ROUND TRIP TIME, 以毫秒顯示
int rtt_c(int arr_time, int rtime, int ttime, int dep_time)
{
   /* 
   arr_time:time the packet returned
   rtime:transmit timestamp
   ttime:receive timestamp
   dep_time:original timestamp
   */
   return (int) ((arr_time - rtime) + (ttime - dep_time));
}
