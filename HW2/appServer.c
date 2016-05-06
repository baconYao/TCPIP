/*
  問題：封包掉得很嚴重
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for gettimeofday function


float calculate (int count,int  size,float timeUse){
  float result;

  printf("Datagram number : %d\n", count);
  printf("Time interval : %f\n", timeUse);

  result = ((float)count * 8 * (float)size)/(1024.0 * 1024.0);   //單位:Mb
  timeUse = timeUse / 1000000.0;    //單位:秒

  result = result / timeUse;      //Throughput

  return result;
}


int main(int argc, char *argv[])
{
  struct sockaddr_in server_addr,client_addr;
  struct timeval start;
  struct timeval end;
  int sock,byte_recv,server_addr_length = sizeof(server_addr),client_addr_length = sizeof(client_addr),recfd;
  int datagramCount = 0;
  char buffer[50];
  char overSignal[10] = "Complete!";
  int timeSwitch = 1;
  float timeUse = 0;


  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    printf("Error creating socket\n");
  }

  bzero(&server_addr, server_addr_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sock, (struct sockaddr *)&server_addr, server_addr_length) == -1)
  {
    printf("Error binding\n");
    close(sock);
  }

  if(listen(sock, 20) == -1)
  {
    printf("Listen failed!\n");
    close(sock);
  }

  printf("Waiting for reveiving\n");
  printf("--------------------------------------------\n");
  for(;;)
  {
    if((recfd = accept(sock,(struct sockaddr *)&client_addr,&client_addr_length))==-1) 
    {
      printf("Accept failed!\n");
      close(sock);
    }
    else
    {
      while(1)
      {
        byte_recv = recv(recfd, buffer, sizeof(buffer), 0);
        
        if(byte_recv > 0 && timeSwitch)
        {
          // 開始計時
          gettimeofday(&start,NULL);
          timeSwitch = 0;
        }
        else if(byte_recv < 0)
        {
          printf("Error recving packet\n");
        }

        if(strspn(buffer,overSignal))
        {
          printf("Client send completed!\n");
          // 終止計時
          gettimeofday(&end,NULL);
          timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
          printf("Throughput : %f Mbps\n", calculate(datagramCount, strlen(buffer), timeUse));
          datagramCount = 0;
          timeSwitch = 1;
          printf("--------------------------------------------\n");
          printf("\nWaiting for reveiving\n");
          printf("--------------------------------------------\n");
          break;
        }
        else
        {
          printf("Datagram %d : %s\n",datagramCount,buffer);
          datagramCount++;
        }
      }
    }

  }

  close(sock);
  return 0;
}
