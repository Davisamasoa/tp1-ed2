#ifndef PB_H
#define PB_H

#include "../registro.h"

typedef struct {
    long esq; 
    TipoRegistro arvoreBinaria;
    long dir; 
} Nodo;

int construirArvoreBinaria(char *gerador, char *nomeArquivoArvore);
void pesquisarNaArvore(char *nomeArquivoArvore);

#endif