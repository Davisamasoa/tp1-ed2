#ifndef AV_H
#define AV_H

#include "../acesso sequencial indexado/asi.h"

typedef struct {
    long esq; 
    tipoRegistro arvoreBinaria;
    long dir; 
} Nodo;

int construirArvoreBinaria(char *gerador, char *nomeArquivoArvore);
void pesquisarNaArvore(char *nomeArquivoArvore);

#endif