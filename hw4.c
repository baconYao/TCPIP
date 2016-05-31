#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>

//#define MAXBUFFERSIZE 20000       //最大的INPUT BUFFER SIZE

unsigned short cksum(unsigned short *, int);    //計算CHECKSUM
int rtt_c(int,int,int,int);     //計算ROUND TRIP TIME

char *hostname;  
int maxcount = 1;         //預設REQUEST次數為5次

int main(int argc, char *argv[])
{
   int sock;
 
   // struct sockaddr_in address;
   // int address_length = sizeof(address);


   struct in_addr taraddr;
   struct sockaddr_in to;
   char buffer[65535];
   struct icmp *icmp;
   struct ip *ip;
   struct timeval start_time;
   int s_time=0,r_time=0;
   struct timeval curr_time;
   struct timeval timeout;
   int seqnum = 0;           //SEQ Number從0開始
   int buffersize = 32;      //預設Buffer Size為32 bytes (20:IP Header, 8:ICMP Header , 4: Original timestamp)
   struct hostent *hp;
   int rtt=0;
   char *hostip;
   int i, j;
   char *pbuffer;

   if(argc < 2) {            //輸入格式錯誤
      printf("Format:sudo ./pingHW IP_Address\n");
      exit(0);
   }
   // argc--;
   i = 1;

   //各個參數的擷取
   /*while(argc > 1) {
       if(strcmp(argv[i], "-c") == 0) {     //設定REQUEST次數
         argc--;
         i++;
         maxcount = atoi(argv[i]);
      }
       if(strcmp(argv[i], "-l") == 0) {     //設定INPUT BUFFER SIZE
         argc--;
         i++;
         if((buffersize = atoi(argv[i])) > MAXBUFFERSIZE) {
            printf("Bad value for option -l, valid range is from 0 to 20000\n");
            exit(1);
         }
      }
      argc--;
      i++;
   }*/
 
   sock = socket(AF_INET, SOCK_RAW, 1);
   if(sock == -1)
   {
      perror("Socket can't creat!\n");
      exit(1);
   }
   hp = gethostbyname(argv[i]);           //查詢hostname的地址
   if(hp) {
      hostname = hp->h_name;
      to.sin_family = hp->h_addrtype;
      // to.sin_port = htons(5555);
      bcopy(hp->h_addr,(caddr_t)&to.sin_addr,hp->h_length);
      for(j = 0; hp->h_addr_list[j]; j++) {
         memcpy(&taraddr.s_addr, hp->h_addr_list[j], hp->h_length);
         hostip = inet_ntoa(taraddr);   
      }
   }
   else {                                 //DESTINATION輸入錯誤
      printf("ping: unknown host %s\n", argv[i]);
      exit(1);
   }

   // test
   // bzero(&address, sizeof(address));
   // address.sin_family = AF_INET;
   // // 如果 = 0，則代表自動分配port number
   // address.sin_port = htons(5555);
   // // 自動填入本機位址
   // address.sin_addr.s_addr = INADDR_ANY;

   // // bind():給訂socket一個名稱
   // if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1){
   //   printf("error binding\n");
   //   close(sock);
   // }
   // test



   timeout.tv_sec = 5;                    //TIMEOUT設為五秒
   timeout.tv_usec = 0;

   printf("Pining %s\n\n", argv[i]);

   //送出和接收ICMP封包
   // for(i = 0; i < maxcount; i++) {
      bzero(buffer,buffersize + 16);         //將Buffer全填零
      // pbuffer = buffer+20;
      icmp = (struct icmp *)buffer;          //轉為ICMP格式
    
      //ECHO_REQUEST MESSAGEShtol()
      // 填入ICMP的預設內容
      icmp->icmp_type = 13;                  //ICMP Request
      icmp->icmp_code = 0;
      icmp->icmp_id = getpid();              //ID設為Process ID
      icmp->icmp_seq = seqnum++;             //SEQ Number從零開始，下一個加一
      //gettimeofday(&start_time,NULL);
      gettimeofday(&start_time, NULL);      //建立時間
      s_time=(start_time.tv_sec % (24*60*60)) * 1000 + (start_time.tv_usec/1000);         //設定並且轉換時間格式，單位:毫秒
      icmp->icmp_otime=htonl(s_time);   //將時間放入icmp_otime內
      icmp->icmp_rtime=0;		//icmp_rtime設為0
      icmp->icmp_ttime=0;		//icmp_ttime設為0
      icmp->icmp_cksum = 0;		//CHECK先填零
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
      }
      // printf("buffer : %ld\n",sizeof(buffer));
      ip = (struct ip *)buffer;
      icmp = (struct icmp*)(buffer + ip->ip_hl*4);
      printf("%d\n", icmp->icmp_rtime);
      printf("%d\n", icmp->icmp_ttime);
      printf("%d\n", s_time);
      //設定Back Time
      gettimeofday(&curr_time, NULL);
      //轉換時間格式
      r_time=(curr_time.tv_sec%(24*60*60))*1000+curr_time.tv_usec/1000;
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

      sleep(1);        //一秒送一次
   // }
   
printf("End of Ping to %s\n",hostip);
   return 0;
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

