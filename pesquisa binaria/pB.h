#ifndef PB_H
#define PB_H

#include "../registro.h"
#include <stdio.h>

typedef struct {
    long esquerda; 
    TipoRegistro registro;
    long direita; 
} Nodo;

// constroi a arvore em arquivo a partir do arquivo de registros.
// Retorna 1 se ok, 0 se erro (arquivo inacessivel ou chave repetida).
int construirArvoreBinaria(char *nomeArquivoRegistros, char *nomeArquivoArvore);

// pesquisa a chave no arquivo da arvore (ja aberto em "rb").
// Retorna 1 se achou (e preenche *registro). Nao imprime nada.
int pesquisarNaArvoreBinaria(FILE *arquivoArvore, int chaveBuscada,
                             TipoRegistro *registro);

#endif
