#ifndef AVB_H
#define AVB_H
#include "../registro.h"
#define M 2
#define MM (2 * M)
#define TRUE 1
#define FALSE 0

typedef struct TipoPagina *TipoApontador;
typedef struct TipoPagina {
  short n;
  TipoRegistro r[MM];
  TipoApontador p[MM + 1];
} TipoPagina;

void Inicializa(TipoApontador *Arvore);
void Insere(TipoRegistro Reg, TipoApontador *Ap);
void InsereNaPagina(TipoApontador Ap, TipoRegistro Reg, TipoApontador ApDir);
void Ins(TipoRegistro Reg, TipoApontador Ap, short *Cresceu,
         TipoRegistro *RegRetorno, TipoApontador *ApRetorno);
void Pesquisa(TipoRegistro *x, TipoApontador Ap);

#endif
