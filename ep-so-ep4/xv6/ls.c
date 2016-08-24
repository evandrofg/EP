#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
/* Não usamos a flag corretamente... */
#define O_NOFOLLOW 1

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], buf_symlink[512], *p;
  int fd, fd_symlink, n;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0, O_NOFOLLOW)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_SYMLINK:
    printf(1, "%s %d %d %d -> ", fmtname(path), st.type, st.ino, st.size);
    fd_symlink = open(path, 0, O_NOFOLLOW);
    while((n = read(fd_symlink, buf_symlink, sizeof(buf_symlink))) > 0)
      write(1, buf_symlink, n);
    printf(1,"\n");
    break;
  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      if (st.type == T_SYMLINK){
        printf(1, "%s %d %d %d -> ", fmtname(buf), st.type, st.ino, st.size);
        fd_symlink = open(buf, 0, O_NOFOLLOW);
        while((n = read(fd_symlink, buf_symlink, sizeof(buf_symlink))) > 0)
          write(1, buf_symlink, n);
        printf(1,"\n");
      }
      else if (st.type != T_SYMLINK)
        printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
