/*
** select.c -- a select() demo
*/
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define STDIN 0 // standard input 的 file descriptor
int main(void)
{
  struct timeval tv;
  fd_set readfds;

  tv.tv_sec = 2;
  tv.tv_usec = 500000;  //0.5 second

  FD_ZERO(&readfds);
  FD_SET(STDIN, &readfds);

  // 不用管 writefds 與 exceptfds：
  select(STDIN+1, &readfds, NULL, NULL, &tv);

  if (FD_ISSET(STDIN, &readfds))
    printf("A key was pressed!\n");
  else
    printf("Timed out.\n");
  return 0;
}
