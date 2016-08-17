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

<<<<<<< HEAD
int getIp(char *);        //取得目的端的IP address
=======
int getIp(void);        //取得目的端的IP address
>>>>>>> f58b47d6a8853efa9e79d8ed47ebe49945c0b35d
unsigned short cksum(unsigned short *, int);    //計算CHECKSUM
int rtt_c(int,int,int,int);     //計算ROUND TRIP TIME

char *hostip;
struct sockaddr_in to;
int to_len = sizeof(to);

int main(int argc, char *argv[])
{
    int sock;
<<<<<<< HEAD
    int seqnum = 0;           //SEQ Number從0開始
    char buffer[65536];
    struct timeval start_time,next_time,back_time;
    int rtt=0;
    int s_time = 0, r_time = 0;
    struct icmp *icmp;
    struct ip *ip = (struct ip *)buffer;
    int buffersize = 32;      //預設Buffer Size為32 bytes (20:IP Header, 8:ICMP Header , 4: Original timestamp)

    //Use ICMP packet
    sock = socket(PF_INET,SOCK_RAW,1);
=======
<<<<<<< HEAD
    int seqnum = 0;           //SEQ Number從0開始
=======
    struct sockaddr_in to;
    int to_len = sizeof(to);
>>>>>>> b6a8d6d52358c329c1b16ec314e44dda4fa50ba8
    char buffer[65536];
    struct timeval start_time,next_time,back_time;
    int rtt=0;
    int s_time = 0, n_time = 0;
    struct icmp *icmp;
    struct ip *ip = (struct ip *)buffer;

<<<<<<< HEAD
    //Use ICMP packet
    sock(PF_INET,SOCK_RAW,1);
>>>>>>> f58b47d6a8853efa9e79d8ed47ebe49945c0b35d
    if(sock == -1)
    {
        perror("Socket can't creat!\n");
        exit(1);
    }

    // 取得Address
<<<<<<< HEAD
    if(getIp(*argv) == 0)
    {
        printf("Can't get host.");
        exit(1);
    }
=======
    if(getIp() == 0)
    {
        exit(1);
    }
=======

    sock(PF_INET,SOCK_RAW,1);


>>>>>>> b6a8d6d52358c329c1b16ec314e44dda4fa50ba8

    printf("Pining %s\n\n", argv[1]);
    //將Buffer初始化
    bzero(buffer,buffersize + 16);
    //轉為ICMP格式
    icmp = (struct icmp *)buffer;
    
>>>>>>> f58b47d6a8853efa9e79d8ed47ebe49945c0b35d

    printf("Pining %s\n\n", argv[1]);
    //將Buffer初始化
    bzero(buffer,buffersize + 16);
    //轉為ICMP格式
    icmp = (struct icmp *)buffer;
    
    // 填入ICMP的預設內容
    icmp->icmp_type = 13;                  //ICMP Request
    icmp->icmp_code = 0;
    icmp->icmp_id = getpid();              //ID設為Process ID
    icmp->icmp_seq = seqnum++;             //SEQ Number從零開始，下一個加一
    //gettimeofday(&start_time,NULL);
    gettimeofday(&start_time, NULL);      //建立時間
    //設定並且轉換時間格式，單位:毫秒
    s_time=(start_time.tv_sec % (24*60*60)) * 1000 + (start_time.tv_usec/1000);
    icmp->icmp_otime=htonl(s_time);   //將時間放入icmp_otime內
    icmp->icmp_rtime=0;   //icmp_rtime設為0
    icmp->icmp_ttime=0;   //icmp_ttime設為0
    icmp->icmp_cksum = 0;   //CHECK先填零
    icmp->icmp_cksum = cksum((unsigned short *)icmp, 16);       //計算CHECKSUM

    sendto(sock, icmp, buffersize + 16, 0, (struct sockaddr *)&to, sizeof(to));   //送出封包
    
    //事前的輸出
    printf("Send request to %s\n\n",hostip);
    printf("icmp_type:%d,icmp_code:%d\n",icmp->icmp_type, icmp->icmp_code);
    printf("Original time:%d\n",s_time);
    printf("Receive time:%d\n",icmp->icmp_rtime);
    printf("Transmit time:%d\n",icmp->icmp_ttime);
    printf("Previous back time:%d\n",r_time);
    printf("Previous RTT:%d\n",rtt);
    printf("----------------------\n");
    
    int recv;
    
    recv = recvfrom(sock, buffer,sizeof(buffer), 0, NULL, NULL);       //接收封包
    if(recv < 0)
    {
       printf("Receive Error!\n");
       exit(1);
    }
    // 將data轉為IP格式的封包
    ip = (struct ip *)buffer;
    //ip_hl * 4 = 20 Byte.Skip IP header
    icmp = (struct icmp*)(buffer + ip->ip_hl*4);
    // printf("%d\n", icmp->icmp_rtime);
    // printf("%d\n", icmp->icmp_ttime);
    // printf("%d\n", s_time);
    //設定Back Time
    gettimeofday(&back_time, NULL);
    //轉換時間格式
    r_time=(back_time.tv_sec%(24*60*60))*1000+back_time.tv_usec/1000;
    //計算RTT
    rtt = rtt_c(r_time,icmp->icmp_rtime,icmp->icmp_ttime,s_time);
    //輸出
    printf("Reply from %s\n\n",hostip);
    printf("icmp_type:%d,icmp_code:%d\n",icmp->icmp_type, icmp->icmp_code);
    printf("Original time:%d\n",s_time);
    printf("Receive time:%d\n",icmp->icmp_rtime);
    printf("Transmit time:%d\n",icmp->icmp_ttime);
    printf("Back time:%d\n",r_time);
    printf("RTT:%d\n\n",rtt);
    printf("======================\n\n");

    printf("End of Ping to %s\n",hostip);

    

    return 0;
}


<<<<<<< HEAD
int getIp(char *argv)
=======
int getIp()
>>>>>>> f58b47d6a8853efa9e79d8ed47ebe49945c0b35d
{
    char *hostname;
    struct in_addr taraddr;
    struct hostent *hp;
    int j;

<<<<<<< HEAD
    //查詢hostname的地址
    hp = gethostbyname(*(argv + 1));           
=======

    hp = gethostbyname(argv[1]);           //查詢hostname的地址
>>>>>>> f58b47d6a8853efa9e79d8ed47ebe49945c0b35d
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
