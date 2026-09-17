#include "asi.h"
#include <stdio.h>
#include <stdlib.h>

int pesquisa(tipoIndice tab[], int tam, tipoRegistro *item, FILE *arq) {
  tipoRegistro pagina[ITENSPAGINA];
  int i, quantItens;
  long desloc;
  // procura pela página onde o item pode se encontrar
  i = 0;
  while (i < tam && tab[i].chave <= item->chave)
    i++;
  // caso a chave desejada seja menor que a 1a chave, o item
  // não existe no arquivo
  if (i == 0)
    return 0;
  else { // a ultima página pode não estar completa
    if (i < tam)
      quantItens = ITENSPAGINA;
    else {
      fseek(arq, 0, SEEK_END);
      int quant = (ftell(arq) / sizeof(tipoRegistro)) % ITENSPAGINA;
      if (quant == 0)
        quantItens = ITENSPAGINA;
      else
        quantItens = quant;
    }
    // lê a página desejada do arquivo
    desloc = (tab[i - 1].posicao - 1) * ITENSPAGINA * sizeof(tipoRegistro);
    fseek(arq, desloc, SEEK_SET);
    fread(&pagina, sizeof(tipoRegistro), quantItens, arq);
    // pesquisa sequencial na página lida
    for (i = 0; i < quantItens; i++)
      if (pagina[i].chave == item->chave) {
        *item = pagina[i];
        return 1;
      }
    return 0;
  }
}

int acessoSequencial() {
  tipoIndice tabela[MAXTABELA];
  FILE *arq;
  tipoRegistro x;
  int pos;

  // abre o arquivo de dados
  if ((arq = fopen("registros.bin", "rb")) == NULL) {
    printf("Erro na abertura do arquivo\n");
    return 0;
  }

  // gera a tabela de índice das páginas
  pos = 0;
  while (fread(&x, sizeof(x), 1, arq) == 1) {

    if (pos >= MAXTABELA) {
      printf("Erro: arquivo possui mais paginas do que o suportado "
             "(MAXTABELA = %d)\n", MAXTABELA);
      fclose(arq);
      return 0;
    }

    tabela[pos].chave = x.chave;
    tabela[pos].posicao = pos + 1;

    pos++;

    fseek(arq, sizeof(x) * 99, SEEK_CUR);
  }

  fflush(stdout);
  printf("Chave do Registro desejado: ");
  scanf("%d", &x.chave);
  // ativa a função de pesquisa
  if (pesquisa(tabela, pos, &x, arq))
    printf("O Registro %.5000s (chave %d) foi localizado", x.dado2, x.chave);
  else
    printf("O Registro de chave %d nao foi localizado", x.chave);
  fclose(arq);
  return 1;
}