#ifndef AVB_H
#define AVB_H
#include "../registro.h"
#define ORDEM 2
#define MAX_REGISTROS (2 * ORDEM)
#define TRUE 1
#define FALSE 0

typedef struct TipoPagina *TipoApontador;
typedef struct TipoPagina {
  short quantidade;
  TipoRegistro registros[MAX_REGISTROS];
  TipoApontador filhos[MAX_REGISTROS + 1];
} TipoPagina;

void inicializarArvoreB(TipoApontador *raiz);
void inserirNaArvoreB(TipoRegistro registro, TipoApontador *raiz);
void inserirNaPagina(TipoApontador pagina, TipoRegistro registro,
                     TipoApontador filhoDireito);
void inserirRecursivo(TipoRegistro registro, TipoApontador pagina,
                      short *cresceu, TipoRegistro *registroPromovido,
                      TipoApontador *filhoPromovido);
void pesquisarNaArvoreB(TipoRegistro *registro, TipoApontador pagina);

#endif
