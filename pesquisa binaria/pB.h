#ifndef PB_H
#define PB_H

#include "../registro.h"

typedef struct {
    long esquerda; 
    TipoRegistro registro;
    long direita; 
} Nodo;

int construirArvoreBinaria(char *nomeArquivoRegistros, char *nomeArquivoArvore);
void pesquisarNaArvoreBinaria(char *nomeArquivoArvore);

#endif
