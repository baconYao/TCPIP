#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()

int main() {
  char buffer[50] = "hello world\0";
  char buffer2[30];
  char s2[] = "ABCDE" ;
  char *point = "hello world";

  printf("\nbuffer[50] = \"hello world\\0\"\n");
  printf("sizeof(buffer): %ld\n", sizeof(buffer));
  printf("strlen(buffer): %ld\n\n", strlen(buffer));

  printf("\nbuffer2[50];\n");
  printf("sizeof(buffer2): %ld\n", sizeof(buffer2));
  printf("strlen(buffer2): %ld\n\n", strlen(buffer2));

  printf("s2[] = \"ABCDE\"\n");
  printf("sizeof(s2): %ld\n", sizeof(s2));
  printf("strlen(s2): %ld\n\n", strlen(s2));

  printf("*point = \"hello world\"\n");
  printf("sizeof(point): %ld\n", sizeof(point));
  printf("strlen(point): %ld\n\n", strlen(point));

  return 0;
}
