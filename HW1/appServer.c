#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for bzero
#include <unistd.h>  // for close()
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PortNumber 3000

int main(int argc, char *argv[]){
  struct sockaddr_in address;
  int sock,byte_recv,address_length = sizeof(address);
  int datagramCount = 0;
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
  // 如果 = 0，則代表自動分配port number
  address.sin_port = htons(PortNumber);
  // 自動填入本機位址
  address.sin_addr.s_addr = INADDR_ANY;

  if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1){
    printf("error binding\n");
    close(sock);
  }

  while(1){
    byte_recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&address, &address_length);
    // 接收失敗
    if(byte_recv < 0) {
        printf("Error recving packet\n");
    }
    
    // Client傳送完畢
    if(strspn(buffer,overSignal)){
      // 釋放記憶體
      // free(buffer);
      printf("Client send completed!\n");
      datagramCount = 0;
    }
    else{
      printf("Datagram %d : %s\n",datagramCount,buffer);
      datagramCount++;
    }
  }

  close(sock);
  return 0;
}
