#ifndef ASI_H
#define ASI_H

// ACESSO SEQUENCIAL INDEXADO

#define ITENSPAGINA 100
#define MAXTABELA 20000

// definição de uma entrada da tabela de índice das páginas
typedef struct {
  int posicao;
  int chave;
} tipoIndice;

// definição de um item do arquivo de dados
typedef struct {
  int chave;
  long dado1;
  char dado2[5000];
} tipoRegistro;

int acessoSequencial();

#endif