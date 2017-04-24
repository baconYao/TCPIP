#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#include "lib.h"

#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"


void error(char *msg) {
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

/*
  計算Throughput
*/
float calculate_throughput (int count, int  size, float timeUse){
  float result;

  printf("Datagram number : %d\n", count);
  printf("Time interval : %f\n", timeUse);

  result = ((float)count * 8 * (float)size)/(1024.0 * 1024.0);   //單位:Mb
  timeUse = timeUse / 1000000.0;    //單位:秒

  result = result / timeUse;      //Throughput

  return result;
}

/*
  確認參數是否正確
*/
void check_parameters_number(int isServer, int argcT, char *argvT[]) {
  if(isServer) {
    if(argcT < 2 || argcT > 2) {
      printf(RED_BOLD"Oops!\n");
      printf("You need input some parameter.\n");
      printf("Example:  ./a.out [port number]\n"RESET);
      exit(1);
    } else if(argcT == 2) {
      if(atoi(argvT[1]) > 1024 && atoi(argvT[1]) < 65536) {
        printf(YEL_BOLD"Valid port number!\n");
        printf("Port number is: %s\n", argvT[1]);
        printf("========================================\n"RESET);
      } else {
        printf(RED_BOLD"Invalid port number!\n");
        exit(1);
      }
    }
  } else {
    if(argcT <= 2 || argcT > 3) {
      printf(RED_BOLD"Oops!\n");
      printf("You need input some parameter.\n");
      printf("Example:  ./a.out [port number] [server ip address]\n"RESET);
      exit(1);
    } else if(argcT == 3) {
      if(atoi(argvT[1]) > 1024 && atoi(argvT[1]) < 65536) {
        printf(YEL_BOLD"Valid port number!\n");
        printf("Port number is: %s, Server IP address: %s\n", argvT[1], argvT[2]);
        printf("========================================\n"RESET);
      } else {
        printf(RED_BOLD"Invalid port number!\n");
        printf("Port number must between 1025~65535\n"RESET);
        exit(1);
      }
    }
  }
}

/*
  recv接收封包
  解決recv不完全接收的問題
*/
int read_in(int socket, char *buf, int len) {
  char *s = buf;
  int slen = len;
  int c = recv(socket, s, slen, 0);
  // 持續接收，直到沒有字元或者遇到'\n' 或 '\0'
  while((c > 0) && (s[c-1] != '\n') || (c > 0) && (s[c-1] != '\0')) {
    s += c;
    slen -= c;
    c = recv(socket,s ,slen, 0);
  }
  if(c < 0) {
    return c;
  } else if(c == 0) {
    buf[0] = '\0';
  } else {
    s[c-1] = '\0';
  }
  return len - slen;
}

int create_socket() {
  int s = socket(PF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    error("Can't open socket!");
  }

  return s;
}


void bind_to_port(int socket, int port) {
  struct sockaddr_in server_addr;

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //使port能重複被使用
  int reuse = 1;
  if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) {
    error("Can't set the reuse option on the socket!");
  }

  if(bind(socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    error("Can't bind to socket");
  }

}

int write_out(int socket, char *msg) {
  int result = send(socket, msg, strlen(msg), 0);
  if(result) {
    fprintf(stderr, "%s: %s\n", "Error talking to the client", strerror(errno));
  }
  return result;
}


int sendall(int s, char *buf, int *len)
{
  int total = 0; // 我們已經送出多少 bytes 的資料
  int bytesleft = *len; // 我們還有多少資料要送
  int n;

  while(total < *len) {
    n = send(s, buf+total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }

  *len = total; // 傳回實際上送出的資料量

  return n==-1?-1:0; // 失敗時傳回 -1、成功時傳回 0
}







