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

#define BUFFER_SIZE 6
#define TIME_SECTION 300 // 0.3 (ms)

int main(int argc, char *argv[]) {
   struct sockaddr_in server_addr;
   int sock, byte_sent, server_addr_length = sizeof(server_addr), count=0;

   char buffer[BUFFER_SIZE]="hello\0";

   struct timeval start, current;

   if (argc < 3) {
      fprintf(stderr, "Usage: %s ip port\n", argv[0]);
      exit(1);
   }

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock < 0)    printf("Error creating socket\n");

   bzero(&server_addr, server_addr_length);
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(atoi(argv[2]));
   server_addr.sin_addr.s_addr = inet_addr(argv[1]);

   if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
      printf("connect failed!");
      exit(1);
   }

   gettimeofday(&start, NULL);

   byte_sent = send(sock, "start", sizeof("start"), 0);
   if (byte_sent < 0)
      printf("Error sending start packet\n");
   else
      count++;

   while(1){
      byte_sent = send(sock, buffer, BUFFER_SIZE, 0);
        
      if (byte_sent < 0)
         printf("Error sending packet\n");
      else {
         count ++;
         gettimeofday(&current, NULL);
         if ((start.tv_sec * 1000000) + start.tv_usec + TIME_SECTION <= (current.tv_sec * 1000000) + current.tv_usec){
            //printf("start time: %lf (s)\n", (int)start.tv_sec%3600 + (double)start.tv_usec/1000000);
                //printf("current time: %lf (s)\n", (int)current.tv_sec%3600 + (double)current.tv_usec/1000000);
            break;
         }
      }
   }

   byte_sent = send(sock, "exit", sizeof("exit"), 0);
   if (byte_sent < 0)
      printf("Error sending exit packet\n");
   else
      count++;

   gettimeofday(&current, NULL);

   int total_size = sizeof(buffer)*8*count;
   double interval = ((int)current.tv_sec%3600 + (double)current.tv_usec/1000000) - ((int)start.tv_sec%3600 + (double)start.tv_usec/1000000);
   printf("---------------------------------------------------------------------\n");
   printf("The number of data: %d ;Total Size: %d (bits)\n", count, total_size);
   printf("Start time: %lf (s)\n", (int)start.tv_sec%3600 + (double)start.tv_usec/1000000);
   printf("End time: %lf (s)\n", (int)current.tv_sec%3600 + (double)current.tv_usec/1000000);
   printf("Time interval: %lf (s)\n", interval);
   printf("Throughput: %lf (Mbps)\n", (double)total_size/ 1000000 / interval);
   printf("---------------------------------------------------------------------\n");

   close(sock);

   return 0;
   
   /*
   int sockfd , length, retval;
   struct sockaddr_in srvaddr, peeraddr;
   
   char buffer[BUFFER_SIZE], echobuffer[BUFFER_SIZE];
   
   char peerip[20] ;
   int	peerport ;
     
   if (argc != 3) { 
   	fprintf(stderr, "Usage: %s server port\n", argv[0]);
        exit(1) ;
   }
   
   sockfd = socket(PF_INET, SOCK_DGRAM, 0);
   
   if (sockfd < 0) {
   	fprintf(stderr, "Datagram socket ?إߥ???n");
   	exit(1) ;
   }

   bzero(&srvaddr, sizeof(srvaddr)) ;

   srvaddr.sin_family = PF_INET ;
   if (inet_aton(argv[1], &srvaddr.sin_addr) == 0 ) {
   	fprintf(stderr, "UDP Server IP ?榡???~ : %s\n",argv[1]) ;
   	exit(1) ;
   }   
   srvaddr.sin_port = htons(atoi(argv[2]));
   length =sizeof(srvaddr) ;
   
   
   fprintf(stdout, "?п?UDP ?T?? (Enter?䵲??): ");
   bzero(&buffer,BUFFER_SIZE);
   fgets(buffer,BUFFER_SIZE,stdin);
   buffer[strlen(buffer)-1] = '\0' ;
   retval=sendto(sockfd,buffer,strlen(buffer),0,
   			(struct sockaddr *)&srvaddr,length);
   if (retval < 0) {
   	fprintf(stderr, "sendto function ?o?Ϳ??~,?L?k?e?X UDP ?T??!!\n");
   	exit(1) ;
   }
   fprintf(stdout, "?e?X UDP ?T??: %s [?e??%s:%d]\n", buffer, argv[1], atoi(argv[2])) ;
   
   bzero(&echobuffer,BUFFER_SIZE) ;
   retval = recvfrom(sockfd,echobuffer,BUFFER_SIZE,0,
   			(struct sockaddr *)&peeraddr, &length);
   if (retval < 0) {
   	fprintf(stderr, "recvfrom function ?o?Ϳ??~,?L?o???? UDP ?T??!!\n");
   	exit(1) ;
   }
   
   bzero(&peerip,20) ;
   strcpy(peerip, inet_ntoa(peeraddr.sin_addr)) ;
   peerport = ntohs(peeraddr.sin_port) ; 
   fprintf(stdout, "????UDP ?T??: %s [?Ӧ?%s:%d]\n",echobuffer, peerip, peerport);
   close(sockfd) ;
   exit(0) ;
   */
}