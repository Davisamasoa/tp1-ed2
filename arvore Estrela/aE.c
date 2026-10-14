#include "aE.h"
#include "../metricas.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

//inicializa a árvore definindo o ponteiro da raiz como nulo (árvore vazia)
void inicializarAB(TipoApontadorEstrela *raiz) { *raiz = NULL; }

//função recursiva que desce até a folha e lida com os splits
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
    
    //procura por qual dos filhos descer na árvore
    //avança enquanto a chave buscada for maior ou igual à chave da página
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           COMP(registro->chave >= pagina->conteudo.interna.chaves[posicao]))
      posicao++;
      
    //chamada recursiva para mergulhar no nó filho correspondente
    inserirRecursivo(registro, pagina->conteudo.interna.filhos[posicao],
                     cresceu, chavePromovida, filhoPromovido);

    //na volta da recursão, se a página de baixo não quebrou, encerra aqui
    if (!*cresceu)
      return;

    //se a página interna atual tem espaço, apenas insere a chave promovida
    if (pagina->conteudo.interna.quantidadeChaves < MAX_CHAVES_INTERNA) {
      posicao = pagina->conteudo.interna.quantidadeChaves;
      //empurra as chaves maiores para a direita para abrir espaço
      while (posicao > 0 &&
             COMP(*chavePromovida < pagina->conteudo.interna.chaves[posicao - 1])) {
        pagina->conteudo.interna.chaves[posicao] = pagina->conteudo.interna.chaves[posicao - 1];
        pagina->conteudo.interna.filhos[posicao + 1] = pagina->conteudo.interna.filhos[posicao];
        posicao--;
      }
      //encaixa a chave promovida e seu ponteiro
      pagina->conteudo.interna.chaves[posicao] = *chavePromovida;
      pagina->conteudo.interna.filhos[posicao + 1] = *filhoPromovido;
      pagina->conteudo.interna.quantidadeChaves++;
      *cresceu = false; 
      return;
    }

    //OVERFLOW: Página interna cheia, junta as MAX+1 chaves em vetores temporários e divide no meio.
    int chaves[MAX_CHAVES_INTERNA + 1];
    TipoApontadorEstrela filhos[MAX_CHAVES_INTERNA + 2];
    int total = MAX_CHAVES_INTERNA + 1;
    
    //copia todos os elementos originais para os vetores temporários
    filhos[0] = pagina->conteudo.interna.filhos[0];
    for (j = 0; j < MAX_CHAVES_INTERNA; j++) {
      chaves[j] = pagina->conteudo.interna.chaves[j];
      filhos[j + 1] = pagina->conteudo.interna.filhos[j + 1];
    }
    
    //abre espaço no vetor temporário e insere a nova chave em ordem
    j = MAX_CHAVES_INTERNA;
    while (j > 0 && COMP(*chavePromovida < chaves[j - 1])) {
      chaves[j] = chaves[j - 1];
      filhos[j + 1] = filhos[j];
      j--;
    }
    chaves[j] = *chavePromovida;
    filhos[j + 1] = *filhoPromovido;

    //calcula o meio matemático para dividir
    int meio = total / 2;

    //cria a nova página interna
    novaPagina = (TipoApontadorEstrela)malloc(offsetof(TipoPaginaEstrela, conteudo) + sizeof(((TipoPaginaEstrela *)0)->conteudo.interna));
    if (novaPagina == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaPagina->tipo = PaginaInterna;
    novaPagina->conteudo.interna.quantidadeChaves = 0;

    //a página velha fica com as chaves do início até o meio
    pagina->conteudo.interna.quantidadeChaves = meio;
    for (j = 0; j < meio; j++) {
      pagina->conteudo.interna.chaves[j] = chaves[j];
      pagina->conteudo.interna.filhos[j + 1] = filhos[j + 1];
    }
    
    //o ponteiro que ficaria logo após a chave do meio vira o filho[0] da nova página
    novaPagina->conteudo.interna.filhos[0] = filhos[meio + 1];
    
    //a nova página recebe a segunda metade das chaves
    for (j = meio + 1; j < total; j++) {
      novaPagina->conteudo.interna.chaves[j - meio - 1] = chaves[j];
      novaPagina->conteudo.interna.filhos[j - meio] = filhos[j + 1];
    }
    novaPagina->conteudo.interna.quantidadeChaves = total - meio - 1;

    //a chave que ficou exatamente no meio é "expulsa" e sobe para o nó pai
    *chavePromovida = chaves[meio];
    *filhoPromovido = novaPagina;
    *cresceu = true;
    
  } 
  else { 
    //procura a posicao correta na folha
    while (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
           COMP(registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave))
      posicao++;
      
    //impede que chaves duplicadas sejam inseridas
    if (posicao <= pagina->conteudo.folha.quantidadeRegistros &&
        COMP(registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave)) {
      printf("Erro! Registro ja esta presente\n");
      *cresceu = false;
      return;
    }
    
    //se a folha atual tem espaço, apenas arrasta os maiores e insere o registro
    if (pagina->conteudo.folha.quantidadeRegistros < MAX_REGISTROS_FOLHA) {
      posicao = pagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < pagina->conteudo.folha.registros[posicao - 1].chave)) {
        pagina->conteudo.folha.registros[posicao] = pagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      pagina->conteudo.folha.registros[posicao] = *registro;
      pagina->conteudo.folha.quantidadeRegistros++;
      *cresceu = false;
      return;
    }

    //OVERFLOW: A folha lotou. Cria uma nova página folha.
    novaPagina = (TipoApontadorEstrela)malloc(sizeof(TipoPaginaEstrela));
    if (novaPagina == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaPagina->tipo = PaginaFolha;
    novaPagina->conteudo.folha.quantidadeRegistros = 0;

    //cria a lista encadeada ligando a folha antiga com a nova
    novaPagina->conteudo.folha.proximo = pagina->conteudo.folha.proximo;
    pagina->conteudo.folha.proximo = novaPagina;

    int meio = MAX_REGISTROS_FOLHA / 2;
    
    //transfere a segunda metade dos registros da velha para a nova folha
    for (j = meio; j < MAX_REGISTROS_FOLHA; j++)
      novaPagina->conteudo.folha.registros[j - meio] = pagina->conteudo.folha.registros[j];
      
    novaPagina->conteudo.folha.quantidadeRegistros = MAX_REGISTROS_FOLHA - meio;
    pagina->conteudo.folha.quantidadeRegistros = meio;

    //decide em qual das duas folhas a nova chave vai parar
    if (COMP(registro->chave < pagina->conteudo.folha.registros[pagina->conteudo.folha.quantidadeRegistros - 1].chave)) {
      //se o novo registro é menor que o último da velha: insere na folha original
      posicao = pagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < pagina->conteudo.folha.registros[posicao - 1].chave)) {
        pagina->conteudo.folha.registros[posicao] = pagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      pagina->conteudo.folha.registros[posicao] = *registro;
      pagina->conteudo.folha.quantidadeRegistros++;
    } else {
      //se onovo registro é maior: insere na folha nova recém-criada
      posicao = novaPagina->conteudo.folha.quantidadeRegistros;
      while (posicao > 0 &&
             COMP(registro->chave < novaPagina->conteudo.folha.registros[posicao - 1].chave)) {
        novaPagina->conteudo.folha.registros[posicao] = novaPagina->conteudo.folha.registros[posicao - 1];
        posicao--;
      }
      novaPagina->conteudo.folha.registros[posicao] = *registro;
      novaPagina->conteudo.folha.quantidadeRegistros++;
    }

    //tira uma "cópia" da primeira placa da folha nova para subir como guia
    //o registro original não pode sumir da folha
    *chavePromovida = novaPagina->conteudo.folha.registros[0].chave;
    *filhoPromovido = novaPagina;
    *cresceu = true; //avisa o nó pai que dividiu
  }
}

//pesquisa a chave na árvore e preenche *registro
int pesquisarNaArvoreBEstrela(TipoRegistro *registro,
                              TipoApontadorEstrela pagina) {
  int posicao;
  if (pagina == NULL)
    return 0; //não achou

  //se for nó interno só serve de guia, não tem os dados completos
  if (pagina->tipo == PaginaInterna) {
    posicao = 1;
    //procura o filho correspondente
    while (posicao < pagina->conteudo.interna.quantidadeChaves &&
           COMP(registro->chave > pagina->conteudo.interna.chaves[posicao - 1]))
      posicao++;
      
    //ajusta se for menor e desce recursivamente
    if (COMP(registro->chave < pagina->conteudo.interna.chaves[posicao - 1]))
      return pesquisarNaArvoreBEstrela(registro, pagina->conteudo.interna.filhos[posicao - 1]);
      
    return pesquisarNaArvoreBEstrela(registro, pagina->conteudo.interna.filhos[posicao]);
  }
  
  //se for folha, chegou nos dados reais
  posicao = 1;
  while (posicao < pagina->conteudo.folha.quantidadeRegistros &&
         COMP(registro->chave > pagina->conteudo.folha.registros[posicao - 1].chave))
    posicao++;
    
  //se encontrou a chave na folha, retorna os dados completos
  if (pagina->conteudo.folha.quantidadeRegistros > 0 &&
      COMP(registro->chave == pagina->conteudo.folha.registros[posicao - 1].chave)) {
    *registro = pagina->conteudo.folha.registros[posicao - 1];
    return 1;
  }
  return 0; //não encontrou
}

//função principal de inserção
void InserirAB(TipoRegistro registro, TipoApontadorEstrela *raiz) {
  bool cresceu;
  int chavePromovida;
  TipoApontadorEstrela filhoPromovido;

  //se a árvore estiver vazia, cria a raiz já como uma página folha pois a chave inserida existe
  if (*raiz == NULL) {
    *raiz = (TipoApontadorEstrela)malloc(sizeof(TipoPaginaEstrela));
    if (*raiz == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    (*raiz)->tipo = PaginaFolha;
    (*raiz)->conteudo.folha.registros[0] = registro; //primeira chave
    (*raiz)->conteudo.folha.quantidadeRegistros = 1;
    (*raiz)->conteudo.folha.proximo = NULL; //a única folha por enquanto
    return;
  }

  inserirRecursivo(&registro, *raiz, &cresceu, &chavePromovida, &filhoPromovido);

  //se a recursão voltou dizendo que cresceu, a raiz lotou e precisamos de uma raiz nova
  if (cresceu) {
    TipoApontadorEstrela novaRaiz = (TipoApontadorEstrela)malloc(offsetof(TipoPaginaEstrela, conteudo) + sizeof(((TipoPaginaEstrela *)0)->conteudo.interna));
    if (novaRaiz == NULL) {
      printf("Erro! Memoria insuficiente\n");
      exit(1);
    }
    novaRaiz->tipo = PaginaInterna; //a nova raiz será sempre um nó interno
    novaRaiz->conteudo.interna.quantidadeChaves = 1;
    novaRaiz->conteudo.interna.chaves[0] = chavePromovida; //a chave que subiu
    novaRaiz->conteudo.interna.filhos[0] = *raiz; //esquerda: raiz antiga
    novaRaiz->conteudo.interna.filhos[1] = filhoPromovido; //direita: página criada no split
    *raiz = novaRaiz; //atualiza o ponteiro principal da árvore
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