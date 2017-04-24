#include "../config.h"
#include "../lib.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc,char *argv[]){
  
  check_parameters_number(0, argc, argv);

  struct sockaddr_in server_addr;
  int byte_sent,server_addr_length = sizeof(server_addr);
  char buffer[6] = "hello\0";
  struct timeval start;
  struct timeval end;
  float timeUse = 0;
  int dataCount = 0;

  // 建立socket
  int sock = create_socket();

  bzero(&server_addr, server_addr_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));
  server_addr.sin_addr.s_addr = inet_addr(argv[2]);

  if(connect(sock, (struct sockaddr *)&server_addr, server_addr_length) == -1)
  {
    printf("connect failed.\n");
    close(sock);
  }

  // 開始計時
  gettimeofday(&start,NULL);
  byte_sent = send(sock, "start", sizeof("start"), 0);
  if(byte_sent < 0) {
      printf("Error sending start packet\n");
  } else {
    dataCount++;
  }

  do
  {
    // int len = strlen(buffer);
    // byte_sent = sendall(sock, buffer, &len);
    byte_sent = send(sock, buffer, sizeof(buffer), 0);
    dataCount++;
    gettimeofday(&end,NULL);
    // 傳送不成功
    if(byte_sent < 0) 
    {
        printf("Error sending packet\n");
    }
    timeUse = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);//微秒
  }while(timeUse < 3000);   //執行0.003秒


  // 結束訊息
  byte_sent = send(sock, "exit", sizeof("exit"), 0);
  if(byte_sent < 0) {
    printf("Error sending packet\n");
  }

  printf("Throughput : %f Mbps\n", calculate_throughput(dataCount, sizeof(buffer), timeUse));

  close(sock);

  return 0;
};

