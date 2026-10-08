#include "aE.h"
#include "../metricas.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void inicializarAB(TipoApontadorEstrela *raiz) { *raiz = NULL; }

static void inserirRecursivo(const TipoRegistro *registro,
                             TipoApontadorEstrela pagina, bool *cresceu,
                             int *chavePromovida,
                             TipoApontadorEstrela *filhoPromovido) {
  long posicao = 1;
  long j;
  TipoApontadorEstrela novaPagina;
  *cresceu = false;
  if (pagina == NULL)
    return;

  if (pagina->tipo == PaginaInterna) {
    posicao = 0;
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           COMP(registro->chave >= pagina->conteudo.interna.chaves[posicao]))
      posicao++;
    inserirRecursivo(registro, pagina->conteudo.interna.filhos[posicao],
                     cresceu, chavePromovida, filhoPromovido);

    if (!*cresceu)
      return;

    if (pagina->conteudo.interna.quantidadeChaves < MAX_CHAVES_INTERNA) {
      /* pagina interna com espaco: insere a chave promovida em ordem */
      posicao = pagina->conteudo.interna.quantidadeChaves;
      while (posicao > 0 &&
             COMP(*chavePromovida < pagina->conteudo.interna.chaves[posicao - 1])) {
        pagina->conteudo.interna.chaves[posicao] =
            pagina->conteudo.interna.chaves[posicao - 1];
        pagina->conteudo.interna.filhos[posicao + 1] =
            pagina->conteudo.interna.filhos[posicao];
        posicao--;
      }
      pagina->conteudo.interna.chaves[posicao] = *chavePromovida;
      pagina->conteudo.interna.filhos[posicao + 1] = *filhoPromovido;
      pagina->conteudo.interna.quantidadeChaves++;
      *cresceu = false;
      return;
    }

    /* pagina cheia: junta as MAX+1 chaves em vetores temporarios e divide no
       meio, assim as duas paginas ficam com pelo menos MAX/2 chaves */
    int chaves[MAX_CHAVES_INTERNA + 1];
    TipoApontadorEstrela filhos[MAX_CHAVES_INTERNA + 2];
    int total = MAX_CHAVES_INTERNA + 1;
    filhos[0] = pagina->conteudo.interna.filhos[0];
    for (j = 0; j < MAX_CHAVES_INTERNA; j++) {
      chaves[j] = pagina->conteudo.interna.chaves[j];
      filhos[j + 1] = pagina->conteudo.interna.filhos[j + 1];
    }
    j = MAX_CHAVES_INTERNA;
    while (j > 0 && COMP(*chavePromovida < chaves[j - 1])) {
      chaves[j] = chaves[j - 1];
      filhos[j + 1] = filhos[j];
      j--;
    }
    chaves[j] = *chavePromovida;
    filhos[j + 1] = *filhoPromovido;

    int meio = total / 2;

    novaPagina = (TipoApontadorEstrela)malloc(offsetof(TipoPaginaEstrela, conteudo) + sizeof(((TipoPaginaEstrela *)0)->conteudo.interna));
    if (novaPagina == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaPagina->tipo = PaginaInterna;
    novaPagina->conteudo.interna.quantidadeChaves = 0;

    pagina->conteudo.interna.quantidadeChaves = meio;
    for (j = 0; j < meio; j++) {
      pagina->conteudo.interna.chaves[j] = chaves[j];
      pagina->conteudo.interna.filhos[j + 1] = filhos[j + 1];
    }
    novaPagina->conteudo.interna.filhos[0] = filhos[meio + 1];
    /* a nova pagina esta vazia e as chaves ja estao ordenadas: so copia */
    for (j = meio + 1; j < total; j++) {
      novaPagina->conteudo.interna.chaves[j - meio - 1] = chaves[j];
      novaPagina->conteudo.interna.filhos[j - meio] = filhos[j + 1];
    }
    novaPagina->conteudo.interna.quantidadeChaves = total - meio - 1;

    *chavePromovida = chaves[meio];
    *filhoPromovido = novaPagina;
    *cresceu = true;
  } else {
    while (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
           COMP(registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave))
      posicao++;
    if (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
        COMP(registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave)) {
      printf("Erro! Registro ja esta presente\n");
      *cresceu = false;
      return;
    }
    if (pagina->conteudo.folha.quantidadeRegistros < MAX_REGISTROS_FOLHA) {
      /* folha com espaco: insere o registro em ordem */
      posicao = pagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < pagina->conteudo.folha.registros[posicao - 1].chave)) {
        pagina->conteudo.folha.registros[posicao] =
            pagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      pagina->conteudo.folha.registros[posicao] = *registro;
      pagina->conteudo.folha.quantidadeRegistros++;
      *cresceu = false;
      return;
    }

    novaPagina = (TipoApontadorEstrela)malloc(sizeof(TipoPaginaEstrela));
    if (novaPagina == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaPagina->tipo = PaginaFolha;
    novaPagina->conteudo.folha.quantidadeRegistros = 0;

    novaPagina->conteudo.folha.proximo = pagina->conteudo.folha.proximo;
    pagina->conteudo.folha.proximo = novaPagina;

    int meio = MAX_REGISTROS_FOLHA / 2;
    /* a nova folha esta vazia e os registros ja estao ordenados: so copia */
    for (j = meio; j < MAX_REGISTROS_FOLHA; j++)
      novaPagina->conteudo.folha.registros[j - meio] =
          pagina->conteudo.folha.registros[j];
    novaPagina->conteudo.folha.quantidadeRegistros =
        MAX_REGISTROS_FOLHA - meio;
    pagina->conteudo.folha.quantidadeRegistros = meio;

    if (COMP(registro->chave < pagina->conteudo.folha.registros[pagina->conteudo.folha.quantidadeRegistros - 1].chave)) {
      /* o novo registro fica na folha original: insere em ordem */
      posicao = pagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < pagina->conteudo.folha.registros[posicao - 1].chave)) {
        pagina->conteudo.folha.registros[posicao] =
            pagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      pagina->conteudo.folha.registros[posicao] = *registro;
      pagina->conteudo.folha.quantidadeRegistros++;
    } else {
      /* o novo registro fica na nova folha: insere em ordem */
      posicao = novaPagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < novaPagina->conteudo.folha.registros[posicao - 1].chave)) {
        novaPagina->conteudo.folha.registros[posicao] =
            novaPagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      novaPagina->conteudo.folha.registros[posicao] = *registro;
      novaPagina->conteudo.folha.quantidadeRegistros++;
    }

    *chavePromovida = novaPagina->conteudo.folha.registros[0].chave;
    *filhoPromovido = novaPagina;
    *cresceu = true;
  }
}

/* pesquisa a chave e preenche *registro. Retorna 1 se achou. Nao imprime. */
int pesquisarNaArvoreBEstrela(TipoRegistro *registro,
                              TipoApontadorEstrela pagina) {
  int posicao;
  if (pagina == NULL)
    return 0;

  if (pagina->tipo == PaginaInterna) {
    posicao = 1;
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           COMP(registro->chave > pagina->conteudo.interna.chaves[posicao - 1]))
      posicao++;
    if (COMP(registro->chave < pagina->conteudo.interna.chaves[posicao - 1]))
      return pesquisarNaArvoreBEstrela(
          registro, pagina->conteudo.interna.filhos[posicao - 1]);
    return pesquisarNaArvoreBEstrela(registro,
                                     pagina->conteudo.interna.filhos[posicao]);
  }
  posicao = 1;
  while (posicao < pagina->conteudo.folha.quantidadeRegistros &&
         COMP(registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave))
    posicao++;
  if (pagina->conteudo.folha.quantidadeRegistros > 0 &&
      COMP(registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave)) {
    *registro = pagina->conteudo.folha.registros[posicao - 1];
    return 1;
  }
  return 0;
}

void InserirAB(TipoRegistro registro, TipoApontadorEstrela *raiz) {
  bool cresceu;
  int chavePromovida;
  TipoApontadorEstrela filhoPromovido;

  if (*raiz == NULL) {
    *raiz = (TipoApontadorEstrela)malloc(sizeof(TipoPaginaEstrela));
    if (*raiz == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    (*raiz)->tipo = PaginaFolha;
    /* primeira folha: o registro vai direto na posicao 0 */
    (*raiz)->conteudo.folha.registros[0] = registro;
    (*raiz)->conteudo.folha.quantidadeRegistros = 1;
    (*raiz)->conteudo.folha.proximo = NULL;
    return;
  }

  inserirRecursivo(&registro, *raiz, &cresceu, &chavePromovida,
                   &filhoPromovido);

  if (cresceu) {
    TipoApontadorEstrela novaRaiz = (TipoApontadorEstrela)malloc(offsetof(TipoPaginaEstrela, conteudo) + sizeof(((TipoPaginaEstrela *)0)->conteudo.interna));
    if (novaRaiz == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaRaiz->tipo = PaginaInterna;
    novaRaiz->conteudo.interna.quantidadeChaves = 1;
    novaRaiz->conteudo.interna.chaves[0] = chavePromovida;
    novaRaiz->conteudo.interna.filhos[0] = *raiz;
    novaRaiz->conteudo.interna.filhos[1] = filhoPromovido;
    *raiz = novaRaiz;
  }
}

static void liberarPagina(TipoApontadorEstrela pagina) {
  int i;
  if (pagina == NULL)
    return;
  if (pagina->tipo == PaginaInterna)
    for (i = 0; i <= pagina->conteudo.interna.quantidadeChaves; i++)
      liberarPagina(pagina->conteudo.interna.filhos[i]);
  free(pagina);
}

void liberarAB(TipoApontadorEstrela *raiz) {
  liberarPagina(*raiz);
  *raiz = NULL;
}
