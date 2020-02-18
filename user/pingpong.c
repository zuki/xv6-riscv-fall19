#include "kernel/types.h"
#include "user.h"

int
main(void)
{
  char buffer[5];
  int pid, p_pc[2], p_cp[2];

  pipe(p_pc);
  pipe(p_cp);
  pid = fork();
  if (pid < 0) {
    printf("could not folk\n");
    exit(1);
  } else if (pid == 0) {
    close(p_pc[1]);
    close(p_cp[0]);
    read(p_pc[0], buffer, 5);
    printf("%d: received %s\n", getpid(), buffer);
    write(p_cp[1], "pong", 5);
  } else {
    close(p_pc[0]);
    close(p_cp[1]);
    write(p_pc[1], "ping", 5);
    read(p_cp[0], buffer, 5);
    printf("%d: received %s\n", getpid(), buffer);
  }
  exit(0);
}
