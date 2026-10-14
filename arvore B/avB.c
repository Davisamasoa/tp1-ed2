#include "avB.h"
#include "../metricas.h"
#include <stdio.h>
#include <stdlib.h>


void inicializarArvoreB(TipoApontador *raiz) { *raiz = NULL; }

void inserirNaArvoreB(TipoRegistro registro, TipoApontador *raiz) {
  short cresceu;
  TipoRegistro registroPromovido;
  TipoPagina *filhoPromovido, *novaRaiz;
  //chama a funcao que localiza o local certo para a chave
  inserirRecursivo(registro, *raiz, &cresceu, &registroPromovido, &filhoPromovido);
  if (cresceu) // Arvore cresce na altura pela raiz 
  //se cresceu volltou como verdadeiro entao a raiz ficou cheia e sera necessario criar outra
  {
    novaRaiz = (TipoPagina *)malloc(sizeof(TipoPagina));
    novaRaiz->quantidade = 1;//apenas o elemento que subiu
    novaRaiz->registros[0] = registroPromovido;//o elemento que fica na raiz
    novaRaiz->filhos[1] = filhoPromovido;//aponta para a pagina que foi criada na divisao
    novaRaiz->filhos[0] = *raiz;//aponta para a raiz antiga da arvore
    *raiz = novaRaiz;//atualiza o ponteiro da raiz da arvore
  }
}
//essa funcao insere uma chave em uma pagina que ja tem espaco garantido
void inserirNaPagina(TipoApontador pagina, TipoRegistro registro,
                     TipoApontador filhoDireito) {
  short procurandoPosicao;
  int posicao;
  posicao = pagina->quantidade;
  procurandoPosicao = (posicao > 0);
  //percorre a pagina de tras para frente movendo os elementos maiores do que o que deve ser inserido uma posicao para a direita
  while (procurandoPosicao) {
    if (COMP(registro.chave >= pagina->registros[posicao - 1].chave)) {
      procurandoPosicao = FALSE;//quando acha a posicao correta sai do laco
      break;
    }
    //coloca o elemento e o seu filho correspondente uma posicao para a direita
    pagina->registros[posicao] = pagina->registros[posicao - 1];
    pagina->filhos[posicao + 1] = pagina->filhos[posicao];
    posicao--;
    if (posicao < 1)
      procurandoPosicao = FALSE;
  }
  //encaixa o novo elemento e seu filho no espaco que foi liberado
  pagina->registros[posicao] = registro;
  pagina->filhos[posicao + 1] = filhoDireito;
  pagina->quantidade++;//incrementa o total de itens da pagina
}
//funcao recursiva que desce ate a folha e lida com os splits
void inserirRecursivo(TipoRegistro registro, TipoApontador pagina,
                      short *cresceu, TipoRegistro *registroPromovido,
                      TipoApontador *filhoPromovido) {
  long posicao = 1;
  long j;
  TipoApontador novaPagina;
  //caso base: chegou no fundo da arvore
  //prepara o registro para ser inserido na pagina anterior 
  if (pagina == NULL) {
    *cresceu = TRUE;
    (*registroPromovido) = registro;
    (*filhoPromovido) = NULL;
    return;
  }
  //procura por qual dos filhos descer na arvore
  while (posicao < pagina->quantidade &&
         COMP(registro.chave > pagina->registros[posicao - 1].chave))
    posicao++;
  //esse if impede que chaves duplicadas sejam inseridas
  if (COMP(registro.chave == pagina->registros[posicao - 1].chave)) {
    printf("Erro: Registro ja esta presente\n");
    *cresceu = FALSE;
    return;
  }
  //ajusta a posicao caso a chave seja menor que a atual sendo analisada
  if (COMP(registro.chave < pagina->registros[posicao - 1].chave))
    posicao--;
  //chamada recursiva para mergulhar no nó filho correspondente
  inserirRecursivo(registro, pagina->filhos[posicao], cresceu,  registroPromovido, filhoPromovido);
  //na volta da recursao, verifica se a pagina de baixo chegou no limite, caso nao, encerra
  if (!*cresceu)
    return;
  //se a pagina atual tem espaco, apenas insere a chave e encerra
  if (pagina->quantidade < MAX_REGISTROS) 
  {
    inserirNaPagina(pagina, *registroPromovido, *filhoPromovido);
    *cresceu = FALSE;
    return;
  }
  //overflow: a pagina tem que ser dividida 
  novaPagina = (TipoApontador)malloc(sizeof(TipoPagina));//cria uma nova pagina
  novaPagina->quantidade = 0;
  novaPagina->filhos[0] = NULL;
  //decide se a chave fica na pagina antiga ou na nova
  if (posicao < ORDEM + 1) {
    //se for ficar na pagina antiga, puxa o ultimo registro da pagina antiga para a nova
    inserirNaPagina(novaPagina, pagina->registros[MAX_REGISTROS - 1],
                    pagina->filhos[MAX_REGISTROS]);
    pagina->quantidade--;//diminui a quantidade de elementos
    //insere a chave na pagina antiga
    inserirNaPagina(pagina, *registroPromovido, *filhoPromovido);
  } else
  //insere o elemento direto na nova pagina
    inserirNaPagina(novaPagina, *registroPromovido, *filhoPromovido);
  //transfere a segunda metade dos elementos da pagina antiga para a nova
  for (j = ORDEM + 2; j <= MAX_REGISTROS; j++)
    inserirNaPagina(novaPagina, pagina->registros[j - 1], pagina->filhos[j]);
  pagina->quantidade = ORDEM;//atualiza a quantidade, a pagina velha agora tem so metade do que tinha antes
  //arruma o primeiro ponteiro da nova pagina, o que vem logo apos a pagina que vai subir
  novaPagina->filhos[0] = pagina->filhos[ORDEM + 1];
  //define o elemento do meio como o elemento que vai subir
  *registroPromovido = pagina->registros[ORDEM];
  *filhoPromovido = novaPagina;//o nó pai deve aponytar para a nova pagina recem criada
}

int pesquisarNaArvoreB(TipoRegistro *registro, TipoApontador pagina) {
  while (pagina != NULL) {
    int esq = 0, dir = pagina->quantidade - 1, meio;

    //busca binária dentro da página
    while (esq <= dir) {
      meio = esq + (dir - esq) / 2;
      //se achou copia os dados completos para a variavel de retorno
      if (COMP(registro->chave == pagina->registros[meio].chave)) {
        *registro = pagina->registros[meio];
        return 1; //encontrou
      }
      //ajusta os limites da busca
      if (COMP(registro->chave < pagina->registros[meio].chave))
        dir = meio - 1;
      else
        esq = meio + 1;
    }
    // Se não encontrou, a variavel esq aponta para o ponteiro do filho correto
    pagina = pagina->filhos[esq];
  }
  return 0; //nao encontrou
}
