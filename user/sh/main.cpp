#include <lib/file.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

namespace {
  bool getCommand(char* buf, int len) {
    printf("$ ");
    memset(buf, 0, len);
    getline(buf, len);
    return buf[0] != '\0';
  }
} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  // int fd;
  // while ((fd = open("console", O_RDWR)) >= 0) {
  //   if(fd >= 3) {
  //     close(fd);
  //     break;
  //   }
  // }
  char buf[1024];
  while (getCommand(buf, sizeof(buf))) {
    printf("cmd: %s\n", buf);
  }
  printf("\n");
  exit(0);
}