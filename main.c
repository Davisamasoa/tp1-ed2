#include "./acesso sequencial indexado/asi.h"
#include "./gerador bin/gerador.h"
#include "./pesquisa binaria/pB.h"
#include "./arvore B/avB.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // GERAR ARQUIVO REGISTROS.BIN
  if (gerarArquivo("registros.bin", 200, 3, 42) != 0) {
    printf("Erro na criação do arquivo\n");
  }

  // acessoSequencialIndexado();
  construirArvoreBinaria("registros.bin", "arvoreb.bin");
  pesquisarNaArvoreBinaria("arvoreb.bin");

  return 0;
}