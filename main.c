#include "./acesso sequencial indexado/asi.h"
#include "./gerador bin/gerador.h"
#include "./pesquisa binaria/av.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // GERAR ARQUIVO REGISTROS.BIN
  if (gerarArquivo("registros.bin", 200, 3, 42) != 0) {
    printf("Erro na criação do arquivo\n");
  }

  // acessoSequencial();
  construirArvoreBinaria("registros.bin", "arvoreb.bin");
  pesquisarNaArvore("arvoreb.bin");

  return 0;
}