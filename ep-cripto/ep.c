/* MAC0336 EP1 - Cryptography using K128
 * Evandro Fernandes Giovanini 5173890 */

#include "crypto.h"
#include "K128.h"

int main(int argc, char **argv){

  int cflag = 0;
  int dflag = 0;
  int aflag = 0;
  char *input = NULL;
  char *output = NULL;
  char pvalue[1024];
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "12cdp:i:o:a")) != -1)
    switch(c)
      {
      case 'c':
        cflag = 1;
        break;
      case 'd':
        dflag = 1;
        break;
      case 'p':
        strcpy (pvalue, optarg);
        break;
      case 'i':
        input = optarg;
        break;
      case 'o':
        output = optarg;
        break;
      case 'a':
        aflag = 1;
        break;
      case '?':
        if (optopt == 'i' || optopt == 'o' || optopt == 'p')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option '-%c'\n.", optopt);
        else
          fprintf (stderr, "Unknown option character '\\x%x'.\n", optopt);
        return 1;
      default:
        abort ();
      }

  /* Initialize variables */
  init_f_y ();
  pvalue[1023] = '\0';

  if (cflag == 1){
    encrypt (input, output, pvalue, aflag);
  }

  if (dflag == 1){
    decrypt (input, output, pvalue);
  }

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  return 0;
}


