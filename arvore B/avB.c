#include "avB.h"
#include <stdio.h>
#include <stdlib.h>

void inicializarArvoreB(TipoApontador *raiz) { *raiz = NULL; }

void inserirNaArvoreB(TipoRegistro registro, TipoApontador *raiz) {
  short cresceu;
  TipoRegistro registroPromovido;
  TipoPagina *filhoPromovido, *novaRaiz;
  inserirRecursivo(registro, *raiz, &cresceu, &registroPromovido,
                   &filhoPromovido);
  if (cresceu) /* Arvore cresce na altura pela raiz */
  {
    novaRaiz = (TipoPagina *)malloc(sizeof(TipoPagina));
    novaRaiz->quantidade = 1;
    novaRaiz->registros[0] = registroPromovido;
    novaRaiz->filhos[1] = filhoPromovido;
    novaRaiz->filhos[0] = *raiz;
    *raiz = novaRaiz;
  }
}
void inserirNaPagina(TipoApontador pagina, TipoRegistro registro,
                     TipoApontador filhoDireito) {
  short procurandoPosicao;
  int posicao;
  posicao = pagina->quantidade;
  procurandoPosicao = (posicao > 0);
  while (procurandoPosicao) {
    if (registro.chave >= pagina->registros[posicao - 1].chave) {
      procurandoPosicao = FALSE;
      break;
    }
    pagina->registros[posicao] = pagina->registros[posicao - 1];
    pagina->filhos[posicao + 1] = pagina->filhos[posicao];
    posicao--;
    if (posicao < 1)
      procurandoPosicao = FALSE;
  }
  pagina->registros[posicao] = registro;
  pagina->filhos[posicao + 1] = filhoDireito;
  pagina->quantidade++;
}
void inserirRecursivo(TipoRegistro registro, TipoApontador pagina,
                      short *cresceu, TipoRegistro *registroPromovido,
                      TipoApontador *filhoPromovido) {
  long posicao = 1;
  long j;
  TipoApontador novaPagina;
  if (pagina == NULL) {
    *cresceu = TRUE;
    (*registroPromovido) = registro;
    (*filhoPromovido) = NULL;
    return;
  }
  while (posicao < pagina->quantidade &&
         registro.chave > pagina->registros[posicao - 1].chave)
    posicao++;
  if (registro.chave == pagina->registros[posicao - 1].chave) {
    printf("Erro: Registro ja esta presente\n");
    *cresceu = FALSE;
    return;
  }
  if (registro.chave < pagina->registros[posicao - 1].chave)
    posicao--;
  inserirRecursivo(registro, pagina->filhos[posicao], cresceu,
                   registroPromovido, filhoPromovido);
  if (!*cresceu)
    return;
  if (pagina->quantidade < MAX_REGISTROS) /* Pagina tem espaco */
  {
    inserirNaPagina(pagina, *registroPromovido, *filhoPromovido);
    *cresceu = FALSE;
    return;
  }
  /* Overflow: Pagina tem que ser dividida */
  novaPagina = (TipoApontador)malloc(sizeof(TipoPagina));
  novaPagina->quantidade = 0;
  novaPagina->filhos[0] = NULL;
  if (posicao < ORDEM + 1) {
    inserirNaPagina(novaPagina, pagina->registros[MAX_REGISTROS - 1],
                    pagina->filhos[MAX_REGISTROS]);
    pagina->quantidade--;
    inserirNaPagina(pagina, *registroPromovido, *filhoPromovido);
  } else
    inserirNaPagina(novaPagina, *registroPromovido, *filhoPromovido);
  for (j = ORDEM + 2; j <= MAX_REGISTROS; j++)
    inserirNaPagina(novaPagina, pagina->registros[j - 1], pagina->filhos[j]);
  pagina->quantidade = ORDEM;
  novaPagina->filhos[0] = pagina->filhos[ORDEM + 1];
  *registroPromovido = pagina->registros[ORDEM];
  *filhoPromovido = novaPagina;
}

void pesquisarNaArvoreB(TipoRegistro *registro, TipoApontador pagina) {
  while (pagina != NULL) {
    int esq = 0, dir = pagina->quantidade - 1, meio;

    // Busca binária dentro da página
    while (esq <= dir) {
      meio = esq + (dir - esq) / 2;

      if (registro->chave == pagina->registros[meio].chave) {
        *registro = pagina->registros[meio];
        return; // Encontrou
      }
      if (registro->chave < pagina->registros[meio].chave)
        dir = meio - 1;
      else
        esq = meio + 1;
    }
    // Se não encontrou, 'esq' aponta para o ponteiro do filho correto
    pagina = pagina->filhos[esq];
  }
  printf("Registro nao esta presente na arvore\n");
}
