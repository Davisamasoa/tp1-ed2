#ifndef ASI_H
#define ASI_H

#include "../registro.h"

// ACESSO SEQUENCIAL INDEXADO

#define ITENS_POR_PAGINA 100
#define MAX_PAGINAS 20000

// definição de uma entrada da tabela de índice das páginas
typedef struct {
  int numeroPagina;
  int primeiraChave;
} TipoIndice;

int acessoSequencialIndexado();

#endif
