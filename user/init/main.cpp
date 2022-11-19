#include <lib/file.h>
#include <lib/stdio.h>
#include <lib/syscall.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  auto stdioPath = "console";
  auto stdio     = open(stdioPath, O_RDWR);
  if (stdio < 0) {
    mknod(stdioPath, DEV_CONSOLE, 0);
    stdio = open(stdioPath, O_RDWR);
  }

  printf("init: starting sh\n");
  auto pid = fork();
  if (pid < 0) {
    printf("init: fork failed\n");
    exit(1);
  }
  if (pid == 0) {
    const char *args[] = { "sh", 0 };
    exec("sh", args);
    printf("init: exec sh failed\n");
    exit(1);
  }
  while(true) {
    // Wait
  }
  exit(0);
}