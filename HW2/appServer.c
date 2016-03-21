#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PortNumber 1234

int main(int argc, char *argv[]){
  struct sockaddr_in server_addr,client_addr;
  int sock,byte_recv,server_addr_length = sizeof(server_addr),client_addr_length = sizeof(client_addr),recfd;
  char buffer[50];

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0){
    printf("Error creating socket\n");
  }

  bzero(&server_addr, server_addr_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PortNumber);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sock, (struct sockaddr *)&server_addr, server_addr_length) == -1){
    printf("Error binding\n");
    close(sock);
  }

  if(listen(sock, 20) == -1){
    printf("Listen failed!\n");
    close(sock);
  }

  while(1){
    if((recfd = accept(sock, (struct sockaddr *)&server_addr, server_addr_length)) == -1){
      printf("accept failed!\n");
      close(sock);
    }

    byte_recv = recv(recfd, buffer, sizeof(buffer), 0);
    if(byte_recv < 0){
      printf("Error recving packet\n");
      printf("data: %s\n",buffer);
    }
  }

  return 0;
}
