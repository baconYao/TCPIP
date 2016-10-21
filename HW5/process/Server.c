/*******************************************************/
/* File  : server_Process.c                            */
/* Usage : ./s.out server port                         */
/*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for bzero()
#include <unistd.h>     //for close() pid_t
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// color defines
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

#define PortNumber 5555
#define BUFFER_SIZE 512
#define MAXMEM 10
#define NAMELEN 20
#define LISTENQ 5


int listenfd;
int clientNumber = 0;// Be used to calculate client number.It'll be add one while client connect successfully

void recv_send(int sd);

int main(int argc, char *argv[])
{

    // the size (int byte) of shared memory object
    const int SIZE = 1024;
    // name of shared memory object
    const char *name = "TCP_PROCESS";
    // shared memory file descriptor
    int shm_fd;
    // pointer to shared object
    void *ptr;
    int clientSd[MAXMEM];


    struct sockaddr_in server_addr, client_addr;
    int byte_recv, server_addr_length = sizeof(server_addr), client_addr_length = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char *bufferPtr = buffer;

    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        printf("Error creating socket\n");
    }
    printf(RED_BOLD"Socket creat successfully\n");

    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PortNumber);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(listenfd, (struct sockaddr *)&server_addr, server_addr_length) == -1)
    {
        printf("error binding!\n");
        close(listenfd);
    }
    printf("Bind successfully\n");

    if(listen(listenfd, LISTENQ) == -1)
    {
        printf("listen failed!\n");
        close(listenfd);
    }
    printf("Listne successfully\n");

    //記錄空閒的客户端的套接字描述符（-1為空閒）
    int i = 0;
    for(i = 0; i < MAXMEM; i++)
    {
        clientSd[i] = -1;
    }


    while(1)
    {
        for(i = 0; i < MAXMEM; i++)
        {
            if(clientSd[i] == -1)
            {
                break;
            }
        }
        if((clientSd[i] = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_length)) == -1)
        {
            printf("accept failed!\n");
            close(listenfd);
        }
        else 
        {
            clientNumber++;
            printf(YEL_BOLD"client %d (Port: %d) connect!\n"RESET, clientNumber, ntohs(client_addr.sin_port));
            
            //create a rcve_send process
            pid_t new_pid;
            new_pid = fork();
            switch(new_pid)
            {
                case -1:
                    printf(RED_BOLD"fork error!\n"RESET);   
                    break;
                case 0: // create a rcve_send process (child)
                    printf(YEL_BOLD"----------------This is child process------------------\n");
                    printf("Pid:%d\nParent Pid:%d\n", getpid(), getppid());
                    close(listenfd);
                    recv_send(clientSd[i]);
                    printf("---------------------------------------------------------\n"RESET);
                    close(clientSd[i]);
                    exit(0);
                    break;
                default:
                    printf(CYAN_BOLD_ITALIC"\n--------------------------------------------\n");
                    printf("Pid:%d\nChild Pid:%d\n",getpid(),new_pid);
                    printf("--------------------------------------------\n"RESET);

                    break;
            }

        }
        
    }


    return 0;
}

void recv_send(int sd)
{
    printf("hello world %d !\n",sd);
}
