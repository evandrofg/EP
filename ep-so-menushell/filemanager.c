#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "filemanager.h"
#define MODE(z) (( file_stat.st_mode & (z)) == (z))


int
list (char *s)
{
	int contador = 0;
	DIR *dp;
	struct dirent *ep;

	struct stat file_stat;
	struct tm *ts;

	dp = opendir (s);
	if (dp != NULL)
	{
		printf("Nome              Tamanho(K)        Tipo&Permissões   Data\n");
		while ((ep = readdir(dp)))
		{
			if (lstat(ep->d_name, &file_stat) == -1)
				printf("Erro.\n");
			else
			{
				printf("%-18s", ep->d_name);
				printf("%-18lu", file_stat.st_size/1024);

				if (ep->d_type == DT_FIFO)
					printf ("p");
				if (ep->d_type == DT_CHR)
					printf ("c");
				if (ep->d_type == DT_DIR)
					printf ("d");
				if (ep->d_type == DT_BLK)
					printf ("b");
				if (ep->d_type == DT_REG)
					printf ("-");
				if (ep->d_type == DT_LNK)
					printf ("l");
				if (ep->d_type == DT_SOCK)
					printf ("s");
				if (ep->d_type == DT_WHT)
					printf ("WHT: ");

				/* Permissões */
				putchar(MODE(S_IRUSR)? 'r' : '-');
				putchar(MODE(S_IWUSR)? 'w' : '-');
				putchar(MODE(S_IXUSR)? 'x' : '-');

				putchar(MODE(S_IRGRP)? 'r' : '-');
				putchar(MODE(S_IWGRP)? 'w' : '-');
				putchar(MODE(S_IXGRP)? 'x' : '-');

				putchar(MODE(S_IROTH)? 'r' : '-');
				putchar(MODE(S_IWOTH)? 'w' : '-');
				putchar(MODE(S_IXOTH)? 'x' : '-');
				
				printf("        ");

				ts=localtime(&file_stat.st_atime);
				printf("%s", asctime(ts));
			}
			contador++;
		}
		(void) closedir (dp);
	}
	else
	{
		perror ("ERRO: Não foi possível abrir o diretório");
		printf ("(argumento \"%s\" inválido)\n", s);
	}
	return contador;
}

