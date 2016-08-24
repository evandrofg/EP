#include "types.h"
#include "stat.h"
#include "user.h"

/* fetchrecords(int arg)
 * if arg = -1, returns the number of syscalls.
 * if arg >= 0, returns the type of syscall stored in position "arg".
 * A total of 256 positions are used for storing data.
 * If fetchrecords returns -1, an error occured.
 */
 
	
int
main(int argc, char *argv[])
{
  int pid;

  startrecording();

  /* Make a few system calls */
  printf(1,"hello world\n");
  printf(1,"uptime: %d\n",uptime());
  
  /* Fetch the recordings, print them */
  printf(1,"(parent)total syscalls: %d\n", fetchrecords(-1));

  /* this is not working, we don't know why
  for (i = 0; i < 257; i++)
    printf(1,"fetchrecords(%d): %d\n",i,fetchrecords(i));*/

  /* Enter the child process and print recording data which demonstrates 
   * the child:
   * 1) inherits the recording flag from the parent.
   * 2) starts with a clean slate for the recorded system calls data.
   */
  pid = fork();
  if (pid == 0){
      printf(1, "(child)total syscalls: %d\n", fetchrecords(-1));
      /* Now make a few more syscalls and try again */
      uptime();
      uptime();
      uptime();
      printf(1, "(child)total syscalls: %d\n", fetchrecords(-1));
  }
  wait();
  
  stoprecording();
  exit();
}
