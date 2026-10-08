#ifndef ABESTRELA_H
#define ABESTRELA_H
#include "../registro.h"
#include <stdbool.h>
#define MAX_CHAVES_INTERNA 100   /* maximo de chaves em uma pagina interna */
#define MAX_REGISTROS_FOLHA 100  /* maximo de registros em uma pagina folha */

typedef enum { PaginaInterna, PaginaFolha } TipoCategoriaPagina;

typedef struct TipoPaginaEstrela *TipoApontadorEstrela;
typedef struct TipoPaginaEstrela {
  TipoCategoriaPagina tipo;
  union {
    struct {
      int quantidadeChaves;
      int chaves[MAX_CHAVES_INTERNA];
      TipoApontadorEstrela filhos[MAX_CHAVES_INTERNA + 1];
    } interna;
    struct {
      int quantidadeRegistros;
      TipoRegistro registros[MAX_REGISTROS_FOLHA];
      TipoApontadorEstrela proximo; //encadeamento das folhas
    } folha;
  } conteudo;
} TipoPaginaEstrela;

void inicializarAB(TipoApontadorEstrela *raiz);
void InserirAB(TipoRegistro registro, TipoApontadorEstrela *raiz);
int pesquisarNaArvoreBEstrela(TipoRegistro *registro, TipoApontadorEstrela pagina);
void liberarAB(TipoApontadorEstrela *raiz);

#endif