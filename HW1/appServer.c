#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for bzero
#include <unistd.h>  // for close()
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PortNumber 3000

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
  int sock,byte_recv,address_length = sizeof(address);
  int datagramCount = 0;
  int timeSwitch = 1;
  // 配置記憶體
  // char *buffer = malloc(bufferSize * sizeof(char));
  char buffer[50];
  char overSignal[10] = "Complete!";
    

  // 建立socket
  sock = socket(PF_INET,SOCK_DGRAM,0);
  if(sock < 0) {
    printf("Error creating socket\n");
    exit(1);
  ;}

  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PortNumber);
  address.sin_addr.s_addr = INADDR_ANY;

  // bind():給訂socket一個名稱
  if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1){
    printf("error binding\n");
    close(sock);
  }

  while(1){
    
    byte_recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&address, &address_length);
    // 接收成功
    if(byte_recv > 0 && timeSwitch) {
      // 開始計時
      gettimeofday(&start,NULL);
      timeSwitch = 0;
    }
    else if(byte_recv < 0){
      printf("Error recving packet\n");
    }
    
    // Client傳送完畢
    if(strspn(buffer,overSignal)){
      // 釋放記憶體
      // free(buffer);
      printf("Client send completed!\n");
      // 終止計時
      gettimeofday(&end,NULL);
      timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
      printf("Throughput : %f Mbps\n", calculate(datagramCount, strlen(buffer), timeUse));
      datagramCount = 0;
      timeSwitch = 1;
    }
    else{
      printf("Datagram %d : %s\n",datagramCount,buffer);
      datagramCount++;
    }
  }

  close(sock);
  return 0;
}
