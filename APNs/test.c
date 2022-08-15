#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>

int main()
{
  u_int32_t len1 = 71;
  u_int32_t len2;
  char *buf1 = (char *) malloc(256);
  char *buf2 = (char *) malloc(256);
  
  memcpy(buf1, &len1, 4);

  memcpy(&len2, buf1, 4);

  printf("len2=[%d]\n", len2);

return 0;
}
