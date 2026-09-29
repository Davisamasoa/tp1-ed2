#include "ab*.h"
#include <stdio.h>

void pesquisarNaArvoreBEstrela(TipoRegistro *registro,
                               TipoApontadorEstrela pagina) {
  int posicao;
  if (pagina->tipo == PaginaInterna) {
    posicao = 1;
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           registro->chave > pagina->conteudo.interna.chaves[posicao - 1])
      posicao++;
    if (registro->chave < pagina->conteudo.interna.chaves[posicao - 1])
      pesquisarNaArvoreBEstrela(registro,
                                pagina->conteudo.interna.filhos[posicao - 1]);
    else
      pesquisarNaArvoreBEstrela(registro,
                                pagina->conteudo.interna.filhos[posicao]);
    return;
  }
  posicao = 1;
  while (posicao < pagina->conteudo.folha.quantidadeRegistros &&
         registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave)
    posicao++;
  if (registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave)
    *registro = pagina->conteudo.folha.registros[posicao - 1];
  else
    printf("Registro nao esta presente na arvore\n");
}
