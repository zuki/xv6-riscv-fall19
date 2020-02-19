#include "kernel/types.h"
#include "user.h"

void
panic(char *s, int d)
{
  fprintf(2, "error: %s %d\n", s, d);
  exit(1);
}

void
primeproc(int fd) {
  int r, num, pid, prime, p[2], wfd;

// 左隣から素数をフェッチ
top:
  if ((r = read(fd, &prime, 4)) == 0) {
    close(fd);
    exit(0);
  } else if (r < 4)
    panic("read initial prime", r);

  printf("prime %d\n", prime);
                            // forkできる数には上限がある。2乗した数が35を超えた素数は
  if (prime*prime > 35) {   // もうフィルターべき数がないので印字だけしてforkしない。
    goto top;
  }

  // 右隣のプロセスをフォーク
  if ((r = pipe(p)) < 0)
    panic("pipe in primeproc", r);
  if ((pid = fork()) < 0)
    panic("fork in primeproc", pid);
  if (pid == 0) {
    close(fd);
    close(p[1]);
    fd = p[0];
    goto top;
  }

  close(p[0]);
  wfd = p[1];

  // この素数の合成数をフィルター
  for(;;) {
    if ((r = read(fd, &num, 4)) == 0) {
      close(fd);
      close(wfd);
      exit(0);
    } else if (r < 4) {
      panic("read number from left", r);
    }

    if (num % prime)
      if ((r = write(wfd, &num, 4)) != 4)
        panic("write number to right", r);
  }
  wait(&pid);
  exit(0);
}

int
main(void)
{
  int r, i, pid, p[2];

  if ((r = pipe(p)) < 0)
    panic("pipe", r);

  // 最初のプロセスをフォーク
  if ((pid = fork()) <0)
    panic("fork", pid);

  if (pid == 0) {
    close(p[1]);
    primeproc(p[0]);
  }

  close(p[0]);

  // 対象の数を流し込む
  for (i=2; i<=35; i++) {
    if ((r = write(p[1], &i, 4)) != 4)
      panic("generator write", r);
  }
  close(p[1]);
  wait(&pid);
  exit(0);
}
