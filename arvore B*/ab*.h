#ifndef ABESTRELA_H
#define ABESTRELA_H
#include "../registro.h"
#define MAX_CHAVES_INTERNA 4    /* maximo de chaves em uma pagina interna */
#define MAX_REGISTROS_FOLHA 4   /* maximo de registros em uma pagina folha */

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
    } folha;
  } conteudo;
} TipoPaginaEstrela;

void pesquisarNaArvoreBEstrela(TipoRegistro *registro,
                               TipoApontadorEstrela pagina);

#endif
