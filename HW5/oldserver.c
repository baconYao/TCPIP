#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for bzero()
#include <unistd.h>     //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PortNumber 5555
#define BUFFER_SIZE 6

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int sock, 
        byte_recv, 
        recfd,
        server_addr_length = sizeof(server_addr), 
        client_addr_length = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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
            int i=0;
            for(i = 0; i < 10; i++)
            {
                byte_recv = recv(recfd, &buffer, sizeof(buffer), 0);
                // printf("Recv bytes: %d\n", byte_recv);
                if(byte_recv < 0)
                {
                    printf("ERROR recving packet\n");
                }
                printf("DATA[%d]: %s\n", i, buffer);
                if(i == 2)
                {
                    printf("------------DATA[%d]: %c\n", i, buffer[2]);
                }
            }
        }
        
    }


    return 0;
}
