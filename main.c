#include <stdio.h>
#include <stdlib.h>

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
} tipoItem;

int pesquisa(tipoIndice tab[], int tam, tipoItem *item, FILE *arq) {
  tipoItem pagina[ITENSPAGINA];
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
      int quant = (ftell(arq) / sizeof(tipoItem)) % ITENSPAGINA;
      if (quant == 0)
        quantItens = ITENSPAGINA;
      else
        quantItens = quant;
    }
    // lê a página desejada do arquivo
    desloc = (tab[i - 1].posicao - 1) * ITENSPAGINA * sizeof(tipoItem);
    fseek(arq, desloc, SEEK_SET);
    fread(&pagina, sizeof(tipoItem), quantItens, arq);
    // pesquisa sequencial na página lida
    for (i = 0; i < quantItens; i++)
      if (pagina[i].chave == item->chave) {
        *item = pagina[i];
        return 1;
      }
    return 0;
  }
}

int main() {
  tipoIndice tabela[MAXTABELA];
  FILE *arq;
  tipoItem x;
  int pos, cont;
  // abre o arquivo de dados
  if ((arq = fopen("registros.bin", "rb")) == NULL) {
    printf("Erro na abertura do arquivo\n");
    return 0;
  }

  // gera a tabela de índice das páginas
  cont = 0;
  pos = 0;
  while (fread(&x, sizeof(x), 1, arq) == 1) {

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
  return 0;
}