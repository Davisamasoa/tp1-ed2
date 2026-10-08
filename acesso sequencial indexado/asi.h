#ifndef ASI_H
#define ASI_H

#include "../registro.h"
#include <stdio.h>

// ACESSO SEQUENCIAL INDEXADO

#define ITENS_POR_PAGINA 100
#define MAX_PAGINAS 20000

// definição de uma entrada da tabela de índice das páginas
typedef struct {
  int numeroPagina;
  int primeiraChave;
} TipoIndice;

// cria a tabela de índice (1 entrada por página). Retorna 1 se ok, 0 se erro.
int criarIndiceASI(FILE *arquivo, TipoIndice indice[], int *quantidadePaginas);

// pesquisa o registro (registro->chave) usando a tabela. Retorna 1 se achou.
// Não imprime nada: quem chama decide o que mostrar.
int pesquisarRegistro(TipoIndice indice[], int quantidadePaginas,
                      TipoRegistro *registro, FILE *arquivo);

#endif
