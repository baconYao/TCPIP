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
#define BUFFER_SIZE 100
#define NUM_THREADS 20

char buffer[BUFFER_SIZE];
char *bufferPtr = buffer;
int count;
int clientNumber = 1;// Be used to calculate client number.It'll be add one while client connect successfully

// 傳遞多個參數給thread handler  參考:http://pccts.blogspot.tw/2007/11/pthreadcreate.html
struct threadArg 
{
    int clientSd;
    int sock;
};


void *threadSend(void *arg)
{
    int byte_sent;
    struct threadArg *tArg = (struct threadArg *)arg;
    // show client who join the chat room
    printf(RED_BOLD"----------------------------------------------\n");
    printf("Client: %d Client ID: %lu join!\n",clientNumber, pthread_self());
    printf("clientSd: %d , Sock: %d \n",tArg->clientSd, tArg->sock);
    // show client's Recv function is OK
    printf("Send thread is ready!\n"RESET);
    // byte_sent = send(tArg->sock, "hello\0", sizeof("hello"), 0);    //sizeof("exit") == 5 Bytes, include \0
    // if (byte_sent < 0)
    // {
    //     printf("Error sending exit packet\n");
    // }

    // first join need to send message to every message which is connected before.


    // int dataNumber = 1;
    // while( (byte_recv = recv(clientSd, bufferPtr, sizeof *bufferPtr, 0)) > 0)
    // {
    //     printf("%c", *bufferPtr);
    //     if(*bufferPtr == '\0')
    //     {
    //         printf("\nData[%d]: ",dataNumber);
    //         dataNumber++;
    //     }
    //     bufferPtr += byte_recv;
    // }
    pthread_exit(NULL);
}

void *threadRecv(void *arg)
{
    // show client's Recv function is OK
    printf(RED_BOLD"Recv thread is ready!\n\n"RESET);
    struct threadArg *tArg = (struct threadArg *)arg;
    int byte_recv;
    clientNumber++;

    int dataNumber = 1;
    while( (byte_recv = recv(tArg->clientSd, bufferPtr, sizeof *bufferPtr, 0)) > 0)
    {
        printf("%c", *bufferPtr);
        if(*bufferPtr == '\0')
        {
            printf("\nData[%d]: ",dataNumber);
            dataNumber++;
        }
        bufferPtr += byte_recv;
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int sock, 
        byte_recv, 
        clientSd,
        server_addr_length = sizeof(server_addr), 
        client_addr_length = sizeof(client_addr);
    

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

    // create threads array
    pthread_t threads[NUM_THREADS];
    int pthreadNumber = 0;
    while(1)
    {
        if((clientSd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_length)) == -1)
        {
            printf("accept failed!\n");
            close(sock);
        }
        else 
        {
            // create two thread to service client, one thread for Send the other for Recv.
            int rc;
            
            // 傳遞多個的參數
            struct threadArg arg;
            arg.clientSd = clientSd;
            arg.sock = sock;
            
            // Send thread
            rc = pthread_create(&threads[pthreadNumber], NULL, threadSend, (void *)&arg);
            if (rc){
               printf("ERROR; return code from pthread_create() is %d\n", rc);
               exit(-1);
            }
            pthreadNumber++;
            sleep(1);
            // Recv thread
            rc = pthread_create(&threads[pthreadNumber], NULL, threadRecv, (void *)&arg);
            if (rc){
               printf("ERROR; return code from pthread_create() is %d\n", rc);
               exit(-1);
            }
            pthreadNumber++;
        }
        
    }


    return 0;
}
