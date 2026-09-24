#ifndef ASI_H
#define ASI_H

#include "../registro.h"

// ACESSO SEQUENCIAL INDEXADO

#define ITENSPAGINA 100
#define MAXTABELA 20000

// definição de uma entrada da tabela de índice das páginas
typedef struct {
  int posicao;
  int chave;
} TipoIndice;

int acessoSequencial();

#endif