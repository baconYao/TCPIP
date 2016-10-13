/*******************************************************/
/* File  : Client.c                                    */
/* Usage : Client    server port                       */
/*******************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {
   struct sockaddr_in server_addr;
   int sock, 
       byte_sent, 
       server_addr_length = sizeof(server_addr), 
       count=0;

   char *buffer = "hello";
   // printf("\nstrlent: %d ", strlen(buffer));         //length = 5 , not include \0
   // printf(" sizeof: %d\n", sizeof(buffer));          //length = 8

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
      printf("connect failed!");
      exit(1);
   }

   byte_sent = send(sock, "Begin!123", sizeof("Begin"), 0);          //sizeof("Begin") == 6 Bytes, include \0
   if (byte_sent < 0)
   {
      printf("Error sending start packet\n");
   }
   else
   {
      count++;
   }

   // send 100 messages
   while(count < 100)
   {
      byte_sent = send(sock, buffer, strlen(buffer)+1, 0);           //strlen(buffer)+1 = [buffer message] + \0
        
      if (byte_sent < 0)
      {
         printf("Error sending packet\n");
      }
      else 
      {
         count ++;
      }
   }

   byte_sent = send(sock, "exit", sizeof("exit"), 0);                //sizeof("exit") == 5 Bytes, include \0
   if (byte_sent < 0)
   {
      printf("Error sending exit packet\n");
   }
   else
   {
      count++;
   }

   int total_size = sizeof(buffer)*8*count;
   printf("---------------------------------------------------------------------\n");
   printf("The number of data: %d ;Total Size: %d (bits)\n", count, total_size);
   printf("---------------------------------------------------------------------\n");

   close(sock);

   return 0;
   
}
