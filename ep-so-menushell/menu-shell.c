#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "filemanager.h"

int main(int ac, char *av[]){
	FILE *f=stdin;
	char *string;
	char *delimiter;
	gchar **token_vector = NULL;

	int status = 0;
	
	/* Parâmetro de getline, automaticamente reajustado pela função se necessário */
	size_t len = 256;

	string = malloc(len);
	delimiter = " ,\n";

	/* Prompt de Comandos */
	while (getline(&string,&len,f) > 0){
		printf("> l, o|e|x|c <arquivo>, q. Para ajuda, digite 'h'.\n");
		printf("> Diretório corrente: %s\n", get_current_dir_name());
		token_vector = g_strsplit_set(string,delimiter,-1);
		if (token_vector[0][0] == 'q'){
			printf("> Saindo...\n"); 
			return 0;
		}
		/* Lista tudo */
		else if (token_vector[0][0] == 'l'){
			list (".");
		}
		/* Visualizar arquivo */
		else if(token_vector[0][0] == 'o'){
			if (fork() == 0)
				execl("/bin/cat","cat",token_vector[1],NULL);
			wait(&status);
		}
		/* Editar arquivo */
		else if(token_vector[0][0] == 'e'){
			if (fork() == 0)
				execl("/bin/vi","vi",token_vector[1],NULL);
			wait(&status);
		}
		/* Executar arquivo */
		else if(token_vector[0][0] == 'x'){
			if (fork() == 0)
				execl(token_vector[1],NULL);
			wait(&status);
		}
		/* Ajuda */
		else if (token_vector[0][0] == 'l'){
			list ("> Comandos:\n");
			printf("> l = Listar arquivos\n");
			printf("> o|e|x <arquivo> - Abrir(o), Editar(e), Executar(x) arquivo\n");
			printf("> c <arquivo> - Muda de diretório");
			printf("> q = Sair\n");
			printf("> h = Ajuda\n");
		}
		/* Mudar de diretório */
		else if (token_vector[0][0] == 'c'){
			chdir(token_vector[1]);
		}
		/* Outra opção */
		else {
			printf("> Comando não reconhecido. Opções válidas: loexcq. Use h para ajuda.\n");
		}
	}

	g_strfreev(token_vector);
	free (string);
	return 0;
}
