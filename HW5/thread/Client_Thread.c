/*******************************************************/
/* File  : Client_Thread.c                             */
/* Usage : ./c.out server port                         */
/* Compile: gcc Client_Thread -o c.out -pthread        */
/*******************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
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

#define BUFFER_SIZE 512
#define NAMELEN 20

int sock;
char buffer[100];
char input[100];

void *threadSend();

int main(int argc, char *argv[]) {
   struct sockaddr_in server_addr;
   int server_addr_length = sizeof(server_addr);

   if (argc < 3) {
      fprintf(stderr, "Usage: %s ip port\n", argv[0]);
      exit(1);
   }

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock < 0)
   {
      printf("Error creating socket\n");
   }

   bzero(&server_addr, server_addr_length);
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(atoi(argv[2]));
   server_addr.sin_addr.s_addr = inet_addr(argv[1]);

   if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) 
   {
      printf("connect failed!\n");
      exit(1);
   }

   printf("You can type 「bye」 to leave chatroom!\n\n");

   // create Send thread
   pthread_t ThreadSend;
   int rc;
   
   // Send thread
   rc = pthread_create(&ThreadSend, NULL, threadSend, NULL);
   // rc = pthread_create(&ThreadSend, NULL, threadSend, (void *)&arg);
   if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      close(sock);
      exit(-1);
   }

   // 接收訊息
   char buff[BUFFER_SIZE];
   int len;

   while(1)
   {
      if((len = read(sock, buff, BUFFER_SIZE)) > 0)
      {
         buff[len] = 0;
         printf("\n%s\n\n", buff);
      }
   }

   return 0;
   
}

void *threadSend()
{
    char name[NAMELEN];
    char recvBuff[BUFFER_SIZE];
    gets(name);
    write(sock, name, strlen(name));
    while(1)
    {
        gets(recvBuff);
        write(sock, recvBuff, strlen(recvBuff));
        if(strcmp("bye",recvBuff) == 0)
        {
            exit(0);
        }
    }
}
