/*******************************************************/
/* File  : Server.c (BF_INET)                          */
/* Usage : Server port                                 */
/*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 6



int main(int argc, char *argv[]) {
   struct sockaddr_in server_addr,client_addr;
   int sock, byte_recv, server_addr_length = sizeof(server_addr), client_addr_length=sizeof(client_addr), recfd; 
   
   char buffer[BUFFER_SIZE];

   struct timeval start, end;
   int count=0, total_size=0;
   double roundtrip=0, throughput=0;

   if (argc < 2) {
      fprintf(stderr, "Usage: %s port\n", argv[0]);
      exit(1);
   }

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock < 0)    printf("Error creating socket\n");

   bzero(&server_addr, server_addr_length);
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(atoi(argv[1]));
   server_addr.sin_addr.s_addr = INADDR_ANY;
   /*
   #define INADDR_ANY  ((unsigned long int) 0x00000000) 
   INADDR_ANY allows the server to accept a client connection on any interface, 
   in case the server host has multiple interfaces.*/

   if (bind(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1) {
        printf("Error binding!\n");
        close(sock);
   }

   if (listen(sock, 20) == -1) {
        printf("Listen failed!\n");
        close(sock);
   }

   fprintf(stdout, "Waiting for receiving...\n");

   for (;;){
      if ((recfd = accept(sock,(struct sockaddr *)&client_addr,&client_addr_length))==-1) {
         printf("Accept failed!\n");
         close(sock);
      }
      else{
         while(1){
            byte_recv = recv(recfd, buffer, sizeof(buffer),0);
            
            if (byte_recv < 0)
                printf("Error recving packet\n");
            else{
                count ++;
                if(strcmp(buffer, "exit") == 0){
                  gettimeofday(&end,NULL);
                  // Receive one time (s)
                  roundtrip = ((int)end.tv_sec%3600 + (double)end.tv_usec/1000000) - ((int)start.tv_sec%3600 + (double)start.tv_usec/1000000);
                  // Throughput (Mb/s)
                  total_size += (double)(BUFFER_SIZE*8);
                  throughput = (double)total_size/ 1000000 / roundtrip;

                  printf("data[%d]: %s   ; data size: %d (bits)\n", count, buffer, BUFFER_SIZE*8);
                  printf("---------------------------------------------------------------------\n");
                  printf("Start time: %lf (s)\n", (int)start.tv_sec%3600 + (double)start.tv_usec/1000000);
                  printf("End time: %lf (s)\n", (int)end.tv_sec%3600 + (double)end.tv_usec/1000000);
                  printf("Total size: %d (bits)\n", total_size);
                  printf("Time interval: %lf (s)\n", roundtrip);
                  printf("Throughput: %lf (Mbps)\n", throughput);
                  printf("---------------------------------------------------------------------\n");

                  total_size = 0;
                  count = 0;
                  fprintf(stdout, "Waiting for receiving...\n") ;
                  break;
                }
                else if(strcmp(buffer, "start") == 0){
                      gettimeofday(&start,NULL);
                      total_size += (double)(BUFFER_SIZE*8);
                      printf("data[%d]: %s   ; data size: %d (bits)\n", count, buffer, BUFFER_SIZE*8);
                }
                else{
                  total_size += (double)(BUFFER_SIZE*8);
                  printf("data[%d]: %s   ; data size: %d (bits)\n", count, buffer, BUFFER_SIZE*8);
                }
            }
         }
      }
      
        
   }

   close(sock);

   return 0;

   /*
   int sock, length, fromlen, retval;
   struct sockaddr_in srvaddr, fromaddr ;
   
   char fromip[40] ;
   int	fromport ;
   
   char buffer[BUFFER_SIZE];
   char echobuf[BUFFER_SIZE] ;

   if (argc < 2) {
      fprintf(stderr, "Usage: %s port\n", argv[0]);
      exit(1);
   }
   
   sock=socket(PF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
      fprintf(stderr, "Datagram Socket 開啟失敗!!\n");
      exit(1) ;
   }
   length = sizeof(srvaddr);
   bzero(&srvaddr,length);
   srvaddr.sin_family = PF_INET;
   srvaddr.sin_addr.s_addr = INADDR_ANY;
   srvaddr.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&srvaddr,length)<0) {
       fprintf(stderr, "Socket Bind 錯誤,無法連結 Port : %d", atoi(argv[1]));
       exit(1) ;
   }
   fromlen = sizeof((struct sockaddr_in)fromaddr);
   while (1) {
       fprintf(stdout, "等待 UDP 訊息到來...\n") ;
       bzero(&buffer, BUFFER_SIZE) ;
       bzero(&fromaddr, fromlen) ;
       retval = recvfrom(sock,buffer,BUFFER_SIZE,0,
       		(struct sockaddr *)&fromaddr,&fromlen);
       if (retval < 0) {
       	fprintf(stderr, "recvfrom function 發生錯誤,無法接收UDP 訊息!!\n");
       	exit(1) ;
       }
       
       bzero(&fromip,20) ;
       strcpy(fromip, inet_ntoa(fromaddr.sin_addr)) ;
       fromport = ntohs(fromaddr.sin_port) ;        
       fprintf(stdout, "接收 UDP 訊息: %s [來至 %s:%d]\n",buffer,fromip,fromport);

       bzero(&echobuf, BUFFER_SIZE) ;
       strcpy(echobuf, strupr(buffer)) ;       
       retval = sendto(sock,echobuf,BUFFER_SIZE,
                  0,(struct sockaddr *)&fromaddr,fromlen);
       if (retval < 0) {
       	fprintf(stderr, "Sendto function 發生錯誤,無法傳送UDP 訊習!!\n") ;
       	exit(1) ;
       }
       fprintf(stdout, "回應 UDP 訊息: %s [送至 %s:%d]\n",echobuf,fromip,fromport);
       
   }
   close(sock) ;
   exit(0) ;
   */
}
/*
char* strupr(char *orgstr) 
{
    int  i, upstr_len ;
    if (strcmp(orgstr,"") == 0)
    	return NULL ;
    bzero(&upstr,BUFFER_SIZE) ;
    strcpy(upstr,orgstr) ;
    upstr_len = strlen(upstr) ;
    for (i=0;i<upstr_len;i++) {
    	if (upstr[i] >= 'a' && upstr[i] <= 'z')
    	   upstr[i] -= 0x20 ; 
    }
    return upstr ;
}
*/
