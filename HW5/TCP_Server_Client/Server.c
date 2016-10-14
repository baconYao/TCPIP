#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for bzero()
#include <unistd.h>     //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PortNumber 5555
#define BUFFER_SIZE 100

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int sock, 
        byte_recv, 
        recfd,
        server_addr_length = sizeof(server_addr), 
        client_addr_length = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char *bufferPtr = buffer;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error creating socket\n");
    }

    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PortNumber);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr *)&server_addr, server_addr_length) == -1)
    {
        printf("error binding!\n");
        close(sock);
    }

    if(listen(sock, 20) == -1)
    {
        printf("listen failed!\n");
        close(sock);
    }

    while(1)
    {
        if((recfd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_length)) == -1)
        {
            printf("accept failed!\n");
            close(sock);
        }
        else 
        {
            int dataNumber = 1;
            while( (byte_recv = recv(recfd, bufferPtr, sizeof *bufferPtr, 0)) > 0)
            {
                printf("%c", *bufferPtr);
                if(*bufferPtr == '\0')
                {
                    printf("\nData[%d]: ",dataNumber);
                    dataNumber++;
                }
                bufferPtr += byte_recv;
            }
        }
        
    }


    return 0;
}
