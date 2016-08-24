EP4 - MAC0422 Sistemas Operacionais
Evandro Fernandes Giovanini

    Este EP implementa suporte a links simbólicos no sistema operacional xv6.

    Este arquivo lista o progresso feito no desenvolvimento do EP, indicando os arquivos modificados em cada etapa.

    O EP foi feito a partir do xv6-rev5. O arquivo ep.patch indica todas as mudanças feitas no código do xv6, e o diretório xv6 inclui o código fonte completo, já com as modificações aplicadas.


PARTE 1: Introdução
- Criar tipo T_SYMLINK em stat.h. DONE
- Adicionar flag -s ao ln.c. DONE
- Modificar argumentos da syscall sys_link() em user.h. DONE
- Preparar para a criação de symlinks em sys_link() em sysfile.c. DONE
- Modificar programas (userstress.c) para compatibilidade. DONE


PARTE 2.1: Criação de symlinks na syscall sys_link()
- Criar o symlink no disco com seu inode. DONE
- Colocar o path do target no bloco indicado pelo inode. DONE


PARTE 2.2: Manipulação de Arquivos
- Alterar a sys_open para seguir symlinks. Isso envolve alterar a função namei, que é chamada por open, para seguir links. Isso foi feito através de uma chamada recursiva da própria função namex.

A função open() foi alterada para:

int open(char*, int, int)

O último argumento especifica se open deve seguir links (0) ou não (1).


PARTE 2.3: Visualização
- Modificar o comando ls em ls.c. DONE
- Usar a flag O_NOFOLLOW. DONE


