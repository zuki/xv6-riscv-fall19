#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *name)
{
  char buf[512], tmp[512];
  char *p, *pp;

  int fd;
  struct dirent de;
  struct stat st;

  //printf("path: %s, name: %s\n", path, name);

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat 1 %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: not directory: %s\n", path);
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  // cd a; ../find .. b としたとき、以下2行の有無で以下のようになる。
  //  あり: ./b, ../a/b
  //  なし: ../a/b
  if (!strcmp(path, ".."))
    find(".", name);

  while (read(fd, &de, sizeof(de)) == sizeof(de)){
      if (de.inum == 0)
        continue;
      if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        continue;
      // statを取るにはfullpathが必要
      strcpy(tmp, buf);
      pp = tmp + strlen(buf);
      memmove(pp, de.name, strlen(de.name));
      pp = tmp + strlen(buf) + strlen(de.name);
      *pp = 0;

      if (stat(tmp, &st) < 0) {
        printf("find: cannot stat 2 %s\n", de.name);
        continue;
      }

      switch(st.type) {
        case T_FILE:
          if (!strcmp(de.name, name)) {
            printf("%s\n", tmp);
            return;
          }
          break;
        case T_DIR:
          find(tmp, name);
          break;
      }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "Usage: find path path\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
