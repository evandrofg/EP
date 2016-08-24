#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3 && argc != 4){
    printf(2, "Usage: ln [-s] old new\n");
    exit();
  }
  if ((argc == 3) && (!strcmp(argv[1],"-s"))){
    printf(2, "Usage: ln [-s] old new\n");
    exit();
  }
  if ((argc == 4) && (strcmp(argv[1],"-s"))){
    printf(2, "Usage: ln [-s] old new\n");
    exit();
  }

  if (argc == 3)
    if(link(argv[1], argv[2], 1) < 0)
      printf(2, "link %s %s: failed\n", argv[1], argv[2]);

  if (argc == 4)
    if(link(argv[2], argv[3], 0) < 0)
      printf(2, "link %s %s: failed\n", argv[2], argv[3]);

  exit();
}
