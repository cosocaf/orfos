#include <lib/file.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

namespace {
  bool getCommandString(char* buf, int len) {
    printf("$ ");
    memset(buf, 0, len);
    getline(buf, len);
    if (buf[0] == '\0') {
      return false;
    }

    len = strlen(buf);
    // buf[len - 1]は改行文字のはず
    buf[len - 1] = '\0';
  }
  constexpr uint64_t MAX_CMD_LENGTH = 12;
  constexpr uint64_t MAX_ARG_LENGTH = 12;
  constexpr uint64_t MAX_ARGS       = 31;
  struct Command {
    char cmd[MAX_CMD_LENGTH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
  };

  void parseCommand(const char* cmdStr, int len, Command& cmd) {
    cmd.cmd[0] = '\0';
    for (auto& arg : cmd.args) {
      arg[0] = '\0';
    }

    char buf[128];
    const auto getToken = [&]() {
      auto beg = cmdStr;
      while (*cmdStr) {
        if (*cmdStr == ' ') {
          break;
        }
        ++cmdStr;
      }
      auto size = cmdStr - beg;
      strncpy(buf, beg, size);
      buf[size] = '\0';
      return size;
    };

    while (*cmdStr) {
      auto size = getToken();
      if (size == 0) {
        ++cmdStr;
        continue;
      }
      if (cmd.cmd[0] == '\0') {
        strncpy(cmd.cmd, buf, size);
      } else {
        for (auto& arg : cmd.args) {
          if (arg[0] == '\0') {
            strncpy(arg, buf, size);
            break;
          }
        }
      }
      ++cmdStr;
    }
  }

  void runCommand(const Command& cmd) {
    const char* argv[MAX_ARGS + 1];
    memset(argv, 0, sizeof(argv));

    argv[0] = cmd.cmd;
    for (int i = 0; i < MAX_ARGS; ++i) {
      if (cmd.args[i][0]) {
        argv[i + 1] = cmd.args[i];
      }
    }
    exec(cmd.cmd, argv);
  }
} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  char buf[128];
  Command cmd;
  while (getCommandString(buf, sizeof(buf))) {
    parseCommand(buf, strlen(buf), cmd);
    if (strncmp(cmd.cmd, "cd", strlen(cmd.cmd)) == 0) {
      // TODO: chdir
    } else if (strncmp(cmd.cmd, "exit", strlen(cmd.cmd)) == 0) {
      break;
    } else {
      auto pid = fork();
      if (pid == 0) {
        runCommand(cmd);
      }
      wait(0);
    }
  }
  printf("\n");
  exit(0);
}