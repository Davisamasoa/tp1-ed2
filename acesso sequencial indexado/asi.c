#include "asi.h"
#include <stdio.h>
#include <stdlib.h>

int pesquisarRegistro(TipoIndice indice[], int quantidadePaginas,
                      TipoRegistro *registro, FILE *arquivo) {
  TipoRegistro pagina[ITENS_POR_PAGINA];
  int paginaAlvo, itensNaPagina;
  long deslocamento;
  // procura pela página onde o registro pode se encontrar
  paginaAlvo = 0;
  while (paginaAlvo < quantidadePaginas &&
         indice[paginaAlvo].primeiraChave <= registro->chave)
    paginaAlvo++;
  // caso a chave desejada seja menor que a 1a chave, o registro
  // não existe no arquivo
  if (paginaAlvo == 0)
    return 0;
  else { // a ultima página pode não estar completa
    if (paginaAlvo < quantidadePaginas)
      itensNaPagina = ITENS_POR_PAGINA;
    else {
      fseek(arquivo, 0, SEEK_END);
      int itensRestantes =
          (ftell(arquivo) / sizeof(TipoRegistro)) % ITENS_POR_PAGINA;
      if (itensRestantes == 0)
        itensNaPagina = ITENS_POR_PAGINA;
      else
        itensNaPagina = itensRestantes;
    }
    // lê a página desejada do arquivo
    deslocamento = (indice[paginaAlvo - 1].numeroPagina - 1) *
                   ITENS_POR_PAGINA * sizeof(TipoRegistro);
    fseek(arquivo, deslocamento, SEEK_SET);
    fread(&pagina, sizeof(TipoRegistro), itensNaPagina, arquivo);
    // pesquisa sequencial na página lida
    for (int i = 0; i < itensNaPagina; i++)
      if (pagina[i].chave == registro->chave) {
        *registro = pagina[i];
        return 1;
      }
    return 0;
  }
}

int acessoSequencialIndexado() {
  TipoIndice indice[MAX_PAGINAS];
  FILE *arquivo;
  TipoRegistro registro;
  int quantidadePaginas;

  // abre o arquivo de dados
  if ((arquivo = fopen("registros.bin", "rb")) == NULL) {
    printf("Erro na abertura do arquivo\n");
    return 0;
  }

  // gera a tabela de índice das páginas
  quantidadePaginas = 0;
  while (fread(&registro, sizeof(registro), 1, arquivo) == 1) {

    if (quantidadePaginas >= MAX_PAGINAS) {
      printf("Erro: arquivo possui mais paginas do que o suportado "
             "(MAX_PAGINAS = %d)\n", MAX_PAGINAS);
      fclose(arquivo);
      return 0;
    }

    indice[quantidadePaginas].primeiraChave = registro.chave;
    indice[quantidadePaginas].numeroPagina = quantidadePaginas + 1;

    quantidadePaginas++;

    // pula o restante da página
    fseek(arquivo, sizeof(registro) * (ITENS_POR_PAGINA - 1), SEEK_CUR);
  }

  fflush(stdout);
  printf("Chave do Registro desejado: ");
  scanf("%d", &registro.chave);
  // ativa a função de pesquisa
  if (pesquisarRegistro(indice, quantidadePaginas, &registro, arquivo))
    printf("O Registro %.5000s (chave %d) foi localizado", registro.dado2,
           registro.chave);
  else
    printf("O Registro de chave %d nao foi localizado", registro.chave);
  fclose(arquivo);
  return 1;
}
