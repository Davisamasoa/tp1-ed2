#include "avB.h"
#include <stdio.h>
#include <stdlib.h>

void Inicializa(TipoApontador *Arvore) { *Arvore = NULL; }

void Insere(TipoRegistro Reg, TipoApontador *Ap) {
  short Cresceu;
  TipoRegistro RegRetorno;
  TipoPagina *ApRetorno, *ApTemp;
  Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
  if (Cresceu) /* Arvore cresce na altura pela raiz */
  {
    ApTemp = (TipoPagina *)malloc(sizeof(TipoPagina));
    ApTemp->n = 1;
    ApTemp->r[0] = RegRetorno;
    ApTemp->p[1] = ApRetorno;
    ApTemp->p[0] = *Ap;
    *Ap = ApTemp;
  }
}
void InsereNaPagina(TipoApontador Ap, TipoRegistro Reg, TipoApontador ApDir) {
  short NaoAchouPosicao;
  int k;
  k = Ap->n;
  NaoAchouPosicao = (k > 0);
  while (NaoAchouPosicao) {
    if (Reg.chave >= Ap->r[k - 1].chave) {
      NaoAchouPosicao = FALSE;
      break;
    }
    Ap->r[k] = Ap->r[k - 1];
    Ap->p[k + 1] = Ap->p[k];
    k--;
    if (k < 1)
      NaoAchouPosicao = FALSE;
  }
  Ap->r[k] = Reg;
  Ap->p[k + 1] = ApDir;
  Ap->n++;
}
void Ins(TipoRegistro Reg, TipoApontador Ap, short *Cresceu,
         TipoRegistro *RegRetorno, TipoApontador *ApRetorno) {
  long i = 1;
  long j;
  TipoApontador ApTemp;
  if (Ap == NULL) {
    *Cresceu = TRUE;
    (*RegRetorno) = Reg;
    (*ApRetorno) = NULL;
    return;
  }
  while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
    i++;
  if (Reg.chave == Ap->r[i - 1].chave) {
    printf("Erro: Registro ja esta presente\n");
    *Cresceu = FALSE;
    return;
  }
  if (Reg.chave < Ap->r[i - 1].chave)
    i--;
  Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);
  if (!*Cresceu)
    return;
  if (Ap->n < MM) /* Pagina tem espaco */
  {
    InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    *Cresceu = FALSE;
    return;
  }
  /* Overflow: Pagina tem que ser dividida */
  ApTemp = (TipoApontador)malloc(sizeof(TipoPagina));
  ApTemp->n = 0;
  ApTemp->p[0] = NULL;
  if (i < M + 1) {
    InsereNaPagina(ApTemp, Ap->r[MM - 1], Ap->p[MM]);
    Ap->n--;
    InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
  } else
    InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
  for (j = M + 2; j <= MM; j++)
    InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);
  Ap->n = M;
  ApTemp->p[0] = Ap->p[M + 1];
  *RegRetorno = Ap->r[M];
  *ApRetorno = ApTemp;
}

void Pesquisa(TipoRegistro *x, TipoApontador Ap) {
  long i = 1;
  if (Ap == NULL) {
    printf("TipoRegistro nao esta presente na arvore\n");
    return;
  }
  while (i < Ap->n && x->chave > Ap->r[i - 1].chave)
    i++;
  if (x->chave == Ap->r[i - 1].chave) {
    *x = Ap->r[i - 1];
    return;
  }
  if (x->chave < Ap->r[i - 1].chave)
    Pesquisa(x, Ap->p[i - 1]);
  else
    Pesquisa(x, Ap->p[i]);
}
