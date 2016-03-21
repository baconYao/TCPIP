#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for bzero
#include <unistd.h>  // for close()
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>


#define Server_PortNumber 3000
#define Server_Address "127.0.0.1"

/* 
    count:送了幾筆資料
    size:每筆資料的大小 單位:bytes
    time:傳送資料的總時間
*/
float calculate (int count,int  size,float timeUse){
  float result;

  printf("Datagram number : %d\n", count);
  printf("Time interval : %f\n", timeUse);

  result = ((float)count * 8 * (float)size)/(1024.0 * 1024.0);   //單位:Mb
  timeUse = timeUse / 1000000.0;    //單位:秒

  result = result / timeUse;      //Throughput

  return result;
}


int main(int argc, char *argv[]){
  struct sockaddr_in address;
  struct timeval start;
  struct timeval end;
  float timeUse = 0;
  int sock,byte_sent,address_length = sizeof(address);
  // 欲傳送的內容
  char buffer[6] = "hello\0";
  // printf("%d\n",sizeof(buffer) );
  // 送資料的數量
  int dataCount = 0;
  // int tt = sizeof(buffer);
  // printf("%d\n",tt);

  // 建立socket
  sock = socket(PF_INET,SOCK_DGRAM,0);
  if(sock < 0) {
      printf("Error creating socket\n");
      close(sock);
      exit(1);
  }

  // 填寫sockaddr_in
  // 把一段記憶體區的內容全部設置為0
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(Server_PortNumber);
  // inet_addr:將IPv4字串型態的Address轉成網路字節順序的型態
  address.sin_addr.s_addr = inet_addr(Server_Address);
  // inet_ntoa:將binary code轉成IPv4的Address
  printf("Server IP: %s\n",inet_ntoa(address.sin_addr));

  // 開始計時
  gettimeofday(&start,NULL);
  do{
      // sendto() 用来將數據由指定的socket傳給對方主機
      byte_sent = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&address, address_length);
      dataCount++;
      gettimeofday(&end,NULL);
      // 傳送不成功
      if(byte_sent < 0) {
          printf("Error sending packet\n");
      }
      
      timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
  }while(timeUse < 3000);   //執行0.001秒

  // 結束訊息
  int sent_complete;
  char cbuffer[10] = "Complete!";
  sent_complete = sendto(sock, cbuffer, sizeof(cbuffer), 0, (struct sockaddr*)&address, address_length);

  printf("Throughput : %f Mbps\n", calculate(dataCount, sizeof(buffer), timeUse));


  close(sock);
  return 0;
}
