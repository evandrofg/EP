O objetivo deste EP é implementar novas syscalls no sistema operacional xv6,
que permitem gravar syscalls que tenham sido feitas. Essa funcionalidade é 
essencial na depuração de programas.

Este EP foi baseado no xv6-rev5 distribuído em tarball compactado. 
Arquivos alterados:

- Makefile
Incluir o testrecording na compilação

- proc.c
Inicializa as variáveis de gravação no processo e filho

- proc.h
Define algumas variáveis de gravação no PCB do processo

- syscall.c
Adiciona as três novas syscalls
Faz alterações no PCB quando uma syscall é feita e gravação está ligada

- syscall.h, user.h, usys.S
Adiciona as três novas syscalls

- sysproc.c
Implementação das três novas syscalls

Arquivos criados:
- testrecording.c
Programa teste

