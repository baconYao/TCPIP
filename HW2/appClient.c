#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


float calculate (int count,int  size,float timeUse){
  float result;

  printf("Datagram number : %d\n", count);
  printf("Time interval : %f\n", timeUse);

  result = ((float)count * 8 * (float)size)/(1024.0 * 1024.0);   //單位:Mb
  timeUse = timeUse / 1000000.0;    //單位:秒

  result = result / timeUse;      //Throughput

  return result;
}


int main(int argc,char *argv[]){
  struct sockaddr_in server_addr;
  int sock,byte_sent,server_addr_length = sizeof(server_addr);
  char buffer[12] = "hello world\0";
  struct timeval start;
  struct timeval end;
  float timeUse = 0;
  int dataCount = 0;

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    printf("Error creating socket!\n");
    close(sock);
    exit(1);
  }

  bzero(&server_addr, server_addr_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);

  if(connect(sock, (struct sockaddr *)&server_addr, server_addr_length) == -1)
  {
    printf("connect failed.\n");
    close(sock);
  }

  // 開始計時
  gettimeofday(&start,NULL);
  do
  {
    byte_sent = send(sock, buffer, sizeof(buffer), 0);
    dataCount++;
    gettimeofday(&end,NULL);
    // 傳送不成功
    if(byte_sent < 0) 
    {
        printf("Error sending packet\n");
    }
    timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
  }while(timeUse < 3000);   //執行0.001秒

  // 結束訊息
  int sent_complete;
  char cbuffer[10] = "Complete!";
  sent_complete = send(sock, buffer, sizeof(buffer), 0);

  printf("Throughput : %f Mbps\n", calculate(dataCount, sizeof(buffer), timeUse));

  close(sock);

  return 0;
};

