#include "av.h"
#include <stdio.h>
#include <stdlib.h>

int construirArvoreBinaria(char *gerador, char *nomeArquivoArvore) {
  FILE *arq, *arvore;
  arq = fopen(gerador, "rb");
  arvore = fopen(nomeArquivoArvore, "wb+");

  tipoRegistro Lido;
  Nodo novo, atual;
  long totalNodos = 0;
  long posNovoNodo;

  // Lê registro por registro do arquivo de entrada
  while (fread(&Lido, sizeof(tipoRegistro), 1, arq) == 1) {
    // dados iniciais salvos no arquivo
    novo.esq = -1;
    novo.arvoreBinaria = Lido;
    novo.dir = -1;

    // a posicao do novo nodo sera ao total atual
    posNovoNodo = totalNodos;

    if (posNovoNodo == 0) {
      // insere na raiz pois ainda nao ha nodos
      fwrite(&novo, sizeof(Nodo), 1, arvore);
      totalNodos++;
    } else {
      // a árvore já tem nodos, precisamos achar onde inserir
      long posAtual = 0; // começa a procura pela raiz
      int inserido = 0;

      while (!inserido) {
        // lê o nodo atual para comparar a chave
        fseek(arvore, posAtual * sizeof(Nodo), SEEK_SET);
        fread(&atual, sizeof(Nodo), 1, arvore);

        if (Lido.chave < atual.arvoreBinaria.chave) {
          // vai para a esquerda se a chave for menor
          if (atual.esq == -1) {
            // achou o espaço vazio
            // atualiza o ponteiro esquerdo do pai
            atual.esq = posNovoNodo;
            fseek(arvore, posAtual * sizeof(Nodo), SEEK_SET);
            fwrite(&atual, sizeof(Nodo), 1, arvore);

            // grava o novo nodo lá no final do arquivo
            fseek(arvore, 0, SEEK_END);
            fwrite(&novo, sizeof(Nodo), 1, arvore);
            inserido = 1;
            // incremeta o total de nodos
            totalNodos++;
          } else {
            // enquanto nao achar, continua descendo pela esquerda
            posAtual = atual.esq;
          }
        } else if (Lido.chave > atual.arvoreBinaria.chave) {
          // vai para a direita se a chave for maior
          if (atual.dir == -1) {
            // achou o espaço vazio
            // atualiza o ponteiro direito do pai
            atual.dir = posNovoNodo;
            fseek(arvore, posAtual * sizeof(Nodo), SEEK_SET);
            fwrite(&atual, sizeof(Nodo), 1, arvore);

            // grava o novo nodo lá no final do arquivo
            fseek(arvore, 0, SEEK_END);
            fwrite(&novo, sizeof(Nodo), 1, arvore);
            inserido = 1;
            // incrementa o total de nodos
            totalNodos++;
          } else {
            // continua descendo pela direita enquanto nao achar
            posAtual = atual.dir;
          }
        } else {
          return 0;
        }
      }
    }
  }

  fclose(arq);
  fclose(arvore);
  return 1;
}

void pesquisarNaArvore(char *nomeArquivoArvore) {
  int chaveBuscada;

  printf("Digite o número da chave que deseja buscar: ");
  scanf("%d", &chaveBuscada);

  FILE *arvore;
  arvore = fopen(nomeArquivoArvore, "rb");
  if (arvore == NULL) {
    printf("Erro ao abrir a arvore para pesquisa.\n");
    return;
  }

  fseek(arvore, 0, SEEK_END);
  // verifica se a arvore possui algum nodo
  if (ftell(arvore) == 0) {
    printf("A arvore esta vazia!\n");
    fclose(arvore);
    return;
  }

  long posAtual = 0; // começa da raiz
  Nodo atual;
  int encontrou = 0;

  while (posAtual != -1) {
    // pula para a posição atual e lê o nodo
    fseek(arvore, posAtual * sizeof(Nodo), SEEK_SET);
    fread(&atual, sizeof(Nodo), 1, arvore);

    if (chaveBuscada == atual.arvoreBinaria.chave) {
      printf("Chave %d ENCONTRADA!\n", chaveBuscada);
      encontrou = 1;
      printf("%ld, %s\n", atual.arvoreBinaria.dado1, atual.arvoreBinaria.dado2);
      break;
    } else if (chaveBuscada < atual.arvoreBinaria.chave) {
      posAtual = atual.esq; // desce para a esquerda
    } else {
      posAtual = atual.dir; // desce para a direita
    }
  }

  if (!encontrou) {
    printf("Chave %d NAO encontrada na arvore.\n", chaveBuscada);
  }

  fclose(arvore);
}
