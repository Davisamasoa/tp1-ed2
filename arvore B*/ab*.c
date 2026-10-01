#include "ab*.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void inicializarAB(TipoApontadorEstrela *raiz) { *raiz = NULL; }

/* aloca so o espaco que o tipo de pagina usa: uma pagina interna nao precisa
   reservar os registros da folha (a union teria o tamanho da folha) */
static TipoApontadorEstrela criarPagina(TipoCategoriaPagina tipo) {
  size_t tamanho;
  TipoApontadorEstrela pagina;
  if (tipo == PaginaInterna)
    tamanho = offsetof(TipoPaginaEstrela, conteudo) +
              sizeof(((TipoPaginaEstrela *)0)->conteudo.interna);
  else
    tamanho = sizeof(TipoPaginaEstrela);
  pagina = (TipoApontadorEstrela)malloc(tamanho);
  if (pagina == NULL) {
    printf("Erro! Memoria insuficiente\n");
    exit(1);
  }
  pagina->tipo = tipo;
  if (tipo == PaginaInterna)
    pagina->conteudo.interna.quantidadeChaves = 0;
  else
    pagina->conteudo.folha.quantidadeRegistros = 0;
  return pagina;
}

static void inserirNaPagina(TipoApontadorEstrela pagina,
                            const TipoRegistro *registro,
                            TipoApontadorEstrela filhoDireito, int chave) {
  int posicao;
  if (pagina->tipo == PaginaInterna) {
    posicao = pagina->conteudo.interna.quantidadeChaves;
    while (posicao > 0 &&
           chave < pagina->conteudo.interna.chaves[posicao - 1]) {
      pagina->conteudo.interna.chaves[posicao] =
          pagina->conteudo.interna.chaves[posicao - 1];
      pagina->conteudo.interna.filhos[posicao + 1] =
          pagina->conteudo.interna.filhos[posicao];
      posicao--;
    }
    pagina->conteudo.interna.chaves[posicao] = chave;
    pagina->conteudo.interna.filhos[posicao + 1] = filhoDireito;
    pagina->conteudo.interna.quantidadeChaves++;
  } else {
    posicao = pagina->conteudo.folha.quantidadeRegistros;
    while (posicao > 0 &&
           registro->chave <
               pagina->conteudo.folha.registros[posicao - 1].chave) {
      pagina->conteudo.folha.registros[posicao] =
          pagina->conteudo.folha.registros[posicao - 1];
      posicao--;
    }
    pagina->conteudo.folha.registros[posicao] = *registro;
    pagina->conteudo.folha.quantidadeRegistros++;
  }
}

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
           registro->chave >= pagina->conteudo.interna.chaves[posicao])
      posicao++;
    inserirRecursivo(registro, pagina->conteudo.interna.filhos[posicao],
                     cresceu, chavePromovida, filhoPromovido);

    if (!*cresceu)
      return;

    if (pagina->conteudo.interna.quantidadeChaves < MAX_CHAVES_INTERNA) {
      inserirNaPagina(pagina, NULL, *filhoPromovido, *chavePromovida);
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
    while (j > 0 && *chavePromovida < chaves[j - 1]) {
      chaves[j] = chaves[j - 1];
      filhos[j + 1] = filhos[j];
      j--;
    }
    chaves[j] = *chavePromovida;
    filhos[j + 1] = *filhoPromovido;

    int meio = total / 2;
    novaPagina = criarPagina(PaginaInterna);

    pagina->conteudo.interna.quantidadeChaves = meio;
    for (j = 0; j < meio; j++) {
      pagina->conteudo.interna.chaves[j] = chaves[j];
      pagina->conteudo.interna.filhos[j + 1] = filhos[j + 1];
    }
    novaPagina->conteudo.interna.filhos[0] = filhos[meio + 1];
    for (j = meio + 1; j < total; j++)
      inserirNaPagina(novaPagina, NULL, filhos[j + 1], chaves[j]);

    *chavePromovida = chaves[meio];
    *filhoPromovido = novaPagina;
    *cresceu = true;
  } else {
    while (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
           registro->chave >
               pagina->conteudo.folha.registros[posicao - 1].chave)
      posicao++;
    if (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
        registro->chave ==
            pagina->conteudo.folha.registros[posicao - 1].chave) {
      printf("Erro! Registro ja esta presente\n");
      *cresceu = false;
      return;
    }
    if (pagina->conteudo.folha.quantidadeRegistros < MAX_REGISTROS_FOLHA) {
      inserirNaPagina(pagina, registro, NULL, 0);
      *cresceu = false;
      return;
    }
    novaPagina = criarPagina(PaginaFolha);
    // encadeamento

    int meio = MAX_REGISTROS_FOLHA / 2;
    for (j = meio; j < MAX_REGISTROS_FOLHA; j++) {
      inserirNaPagina(novaPagina, &pagina->conteudo.folha.registros[j], NULL,
                      0);
    }
    pagina->conteudo.folha.quantidadeRegistros = meio;
    if (registro->chave <
        pagina->conteudo.folha
            .registros[pagina->conteudo.folha.quantidadeRegistros - 1]
            .chave)
      inserirNaPagina(pagina, registro, NULL, 0);
    else
      inserirNaPagina(novaPagina, registro, NULL, 0);
    *chavePromovida = novaPagina->conteudo.folha.registros[0].chave;
    *filhoPromovido = novaPagina;
    *cresceu = true;
  }
}

/* retorna true e preenche *registro se a chave estiver na arvore */
bool pesquisarNaArvoreBEstrela(TipoRegistro *registro,
                               TipoApontadorEstrela pagina) {
  int posicao;
  if (pagina == NULL)
    return false;
  if (pagina->tipo == PaginaInterna) {
    posicao = 1;
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           registro->chave > pagina->conteudo.interna.chaves[posicao - 1])
      posicao++;
    if (registro->chave < pagina->conteudo.interna.chaves[posicao - 1])
      return pesquisarNaArvoreBEstrela(
          registro, pagina->conteudo.interna.filhos[posicao - 1]);
    return pesquisarNaArvoreBEstrela(registro,
                                     pagina->conteudo.interna.filhos[posicao]);
  }
  posicao = 1;
  while (posicao < pagina->conteudo.folha.quantidadeRegistros &&
         registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave)
    posicao++;
  if (pagina->conteudo.folha.quantidadeRegistros > 0 &&
      registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave) {
    *registro = pagina->conteudo.folha.registros[posicao - 1];
    return true;
  }
  return false;
}

void InserirAB(TipoRegistro registro, TipoApontadorEstrela *raiz) {
  bool cresceu;
  int chavePromovida;
  TipoApontadorEstrela filhoPromovido;

  if (*raiz == NULL) {
    *raiz = criarPagina(PaginaFolha);
    inserirNaPagina(*raiz, &registro, NULL, 0);
    return;
  }

  inserirRecursivo(&registro, *raiz, &cresceu, &chavePromovida,
                   &filhoPromovido);

  if (cresceu) {
    TipoApontadorEstrela novaRaiz = criarPagina(PaginaInterna);
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
