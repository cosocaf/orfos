#include <lib/stdio.h>
#include <lib/syscall.h>

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    if (i != 1) {
      printf(" ");
    }
    printf(argv[i]);
  }
  printf("\n");
  exit(0);
}