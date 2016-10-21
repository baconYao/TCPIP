/*******************************************************/
/* File  : Server_Thread.c                             */
/* Usage : ./s.out                                     */
/* Compile: gcc Server_Thread -o s.out -pthread        */
/*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for bzero()
#include <unistd.h>     //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>    //for pthread()
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

// char buffer[BUFFER_SIZE];
// char *bufferPtr = buffer;
int clientSd[MAXMEM];
int listenfd;
int clientNumber = 0;// Be used to calculate client number.It'll be add one while client connect successfully

void *quit();
void *rcv_snd(void *arg);


int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int server_addr_length = sizeof(server_addr), client_addr_length = sizeof(client_addr);
    pthread_t closeThread;      //用來關閉server的thread

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

    // 建立用來管理Server的thread
    pthread_create(&closeThread, NULL, quit, NULL);

    printf("You can type 「quit」 to close server!\n\n"RESET);


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
            
            int rc;
            // rcv_snd thread
            rc = pthread_create(malloc(sizeof(pthread_t)), NULL, rcv_snd, (void *)i);
            if (rc){
               printf("ERROR; return code from pthread_create() is %d\n", rc);
               exit(-1);
            }
        }
        
    }


    return 0;
}

// 管理server的thread
void *quit()
{
    char msg[10];
    while(1)
    {
        scanf("%s",msg);
        if(strcmp("quit",msg)==0)
        {
            printf("Server close\n");
            close(listenfd);
            exit(0);
        }
    }
}

// 收送的thread
void *rcv_snd(void *arg)
{
    // struct threadArg *tArg = (struct threadArg *)arg;
    char* askClientName="Your name please：";
    char buff[BUFFER_SIZE];
    char recvBuff[BUFFER_SIZE];
    // char buff2[BUFFER_SIZE];
    char name[NAMELEN];
    // time_t ticks;
    int i = 0;
    int retval;

    // 獲得client的名字
    send(clientSd[(int)arg], askClientName, strlen(askClientName)+1, 0);
    // write(clientSd[tArg->clientSD], askClientName, strlen(askClientName)+1);
    int len;
    len = read(clientSd[(int)arg], name, NAMELEN);
    if(len > 0)
    {
        name[len] = 0;
    }

    // 當一個client加入聊天室時，會告知其他clients
    strcpy(buff,name);
    strcat(buff,"\tjoin in this chatroom!\0");
    for(i = 0; i < MAXMEM; i++)
    {
        if(clientSd[i] != -1)
        {
            write(clientSd[i],buff,strlen(buff));
        }
    }

    while(1)
    {
        if((len = read(clientSd[(int)arg], recvBuff, BUFFER_SIZE)) > 0)
        {
            recvBuff[len] = 0;

            //當client輸入"bye”時，此client退出聊天室
            if(strcmp("bye", buff) == 0)
            {
                close(clientSd[(int)arg]);
                clientSd[(int)arg] = -1;
                pthread_exit(&retval);
            }

            strcpy(buff, name);
            strcat(buff,": ");
            //strcat(buff,buff2);
            strcat(buff,recvBuff);

            printf("%s\n", recvBuff);
            for(i = 0; i < MAXMEM; i++)
            {
                 if(clientSd[i] != -1)
                 {
                      write(clientSd[i], buff, strlen(buff));
                 }
            }
        }

    }
}

