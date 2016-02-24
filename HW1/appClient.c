#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for bzero
#include <unistd.h>  // for close()
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>

#define Server_PortNumber 3000
#define Server_Address "127.0.0.1"

int main(int argc, char *argv[]){
    struct sockaddr_in address;
    int sock,byte_sent,address_length = sizeof(address);
    char buffer[6] = "hello\0";

    sock = socket(PF_INET,SOCK_DGRAM,0);
    if(sock < 0) {printf("Error creating socket\n");}

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(Server_PortNumber);
    address.sin_addr.s_addr = inet_addr(Server_Address);

    byte_sent = sendto(sock, buffer, sizeof(buffer), 0,(struct sockaddr*)&address, address_length);
    if(byte_sent < 0) {printf("Error sending packet\n");}
    close(sock);
    return 0;
}
