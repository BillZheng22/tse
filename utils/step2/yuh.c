#include <stdio.h>
#include <stdlib.h>

void bro(int *gosh){
  printf("Hey man %d, I wanna show you the beautiful world.\n", *gosh);
  *gosh += 1;
}

int main(void) {
  int x = 10;
  while (x < 20) {
    bro(&x);
    if (x == 15.0) {
      printf("yuh didn't make it yet bud.\n");
    }
  }
	bro(NULL);
  exit(EXIT_SUCCESS);
}
