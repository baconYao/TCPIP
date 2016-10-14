#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for bzero()
#include <unistd.h>     //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>    //for pthread()

#define PortNumber 5555
#define BUFFER_SIZE 100
#define NUM_THREADS 20

char buffer[BUFFER_SIZE];
char *bufferPtr = buffer;


void *threadFunction(void *arg)
{

    int dataNumber = 1;
    while( (byte_recv = recv(clientSd, bufferPtr, sizeof *bufferPtr, 0)) > 0)
    {
        printf("%c", *bufferPtr);
        if(*bufferPtr == '\0')
        {
            printf("\nData[%d]: ",dataNumber);
            dataNumber++;
        }
        bufferPtr += byte_recv;
    }


    int i;
    printf("-----------------\n");
    printf("Thread: %d Thread ID: %lu\n",(int) arg,pthread_self());
    for (i = 0; i < 250000; i++)
    {
      count +=1;
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
            // create a thread to service client
            int rc;
            rc = pthread_create(&threads[pthreadNumber], NULL, threadFunction, (void *)pthreadNumber+1);
            if (rc){
               printf("ERROR; return code from pthread_create() is %d\n", rc);
               exit(-1);
            }
            rc = pthread_join(threads[pthreadNumber], NULL);
            if (rc){
               printf("ERROR; return code from pthread_join() is %d\n", rc);
               exit(-1);
            }
            pthreadNumber++;
        }
        
    }


    return 0;
}
