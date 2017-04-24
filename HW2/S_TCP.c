#include "../config.h"
#include "../lib.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for gettimeofday function

int main(int argc, char *argv[])
{
  check_parameters_number(1, argc, argv);

  struct sockaddr_in client_addr;
  struct timeval start;
  struct timeval end;
  int byte_recv,client_addr_length = sizeof(client_addr),recfd;
  int datagramCount = 0;
  char buffer[6];
  float timeUse = 0;

  // 建立socket
  int sock = create_socket();
  // 綁定port number
  bind_to_port(sock, atoi(argv[1]));
  
  if(listen(sock, 20) == -1)
  {
    printf("Listen failed!\n");
    close(sock);
    exit(1);
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
        
        if(byte_recv < 0)
        {
          printf("Error recving packet\n");
        } else {
          datagramCount++;
          if(strcmp(buffer, "start") == 0) {
            gettimeofday(&start,NULL);
          } else if(strcmp(buffer, "exit") == 0){
            printf("Client send completed!\n");
            // 終止計時
            gettimeofday(&end,NULL);
            timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
            printf("Throughput : %f Mbps\n", calculate_throughput(datagramCount, strlen(buffer), timeUse));
            datagramCount = 0;
            printf("--------------------------------------------\n");
            printf("\nWaiting for reveiving\n");
            printf("--------------------------------------------\n");
            break;
          }
          else
          {
            printf("Datagram %d : %s\n",datagramCount,buffer);
          }
        }
      }
    }

  }

  close(sock);
  return 0;
}
