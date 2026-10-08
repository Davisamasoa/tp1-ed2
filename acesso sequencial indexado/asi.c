#include "asi.h"
#include "../metricas.h"
#include <stdio.h>
#include <stdlib.h>

int pesquisarRegistro(TipoIndice indice[], int quantidadePaginas,
                      TipoRegistro *registro, FILE *arquivo) {
  TipoRegistro pagina[ITENS_POR_PAGINA];
  int paginaAlvo, itensNaPagina;
  long deslocamento;
  // procura pela página onde o registro pode se encontrar
  // (cada comparação com a tabela de índice é contada)
  paginaAlvo = 0;
  while (paginaAlvo < quantidadePaginas &&
         COMP(indice[paginaAlvo].primeiraChave <= registro->chave))
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
    size_t lidos = fread(&pagina, sizeof(TipoRegistro), itensNaPagina, arquivo);
    metricas.transferencias += (long)lidos; // registros trazidos para a RAM
    // pesquisa sequencial na página lida
    for (size_t i = 0; i < lidos; i++)
      if (COMP(pagina[i].chave == registro->chave)) {
        *registro = pagina[i];
        return 1;
      }
    return 0;
  }
}

int criarIndiceASI(FILE *arquivo, TipoIndice indice[], int *quantidadePaginas) {
  TipoRegistro registro;

  // gera a tabela de índice das páginas
  *quantidadePaginas = 0;
  while (fread(&registro, sizeof(registro), 1, arquivo) == 1) {
    metricas.transferencias++; // 1 registro lido (o 1o de cada página)

    if (*quantidadePaginas >= MAX_PAGINAS) {
      printf("Erro: arquivo possui mais paginas do que o suportado "
             "(MAX_PAGINAS = %d)\n", MAX_PAGINAS);
      return 0;
    }

    indice[*quantidadePaginas].primeiraChave = registro.chave;
    indice[*quantidadePaginas].numeroPagina = *quantidadePaginas + 1;

    (*quantidadePaginas)++;

    // pula o restante da página
    fseek(arquivo, sizeof(registro) * (ITENS_POR_PAGINA - 1), SEEK_CUR);
  }
  return 1;
}
