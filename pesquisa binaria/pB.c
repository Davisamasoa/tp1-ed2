#include "pB.h"
#include "../metricas.h"
#include "../registro.h"
#include <stdio.h>
#include <stdlib.h>

int construirArvoreBinaria(char *nomeArquivoRegistros, char *nomeArquivoArvore) {
  FILE *arquivoRegistros, *arquivoArvore;
  arquivoRegistros = fopen(nomeArquivoRegistros, "rb");
  if (arquivoRegistros == NULL)
    return 0;
  arquivoArvore = fopen(nomeArquivoArvore, "wb+");
  if (arquivoArvore == NULL) {
    fclose(arquivoRegistros);
    return 0;
  }

  TipoRegistro registroLido;
  Nodo novoNodo, nodoAtual;
  long totalNodos = 0;
  long posNovoNodo;

  // Lê registro por registro do arquivo de entrada
  while (fread(&registroLido, sizeof(TipoRegistro), 1, arquivoRegistros) == 1) {
    metricas.transferencias++; // registro lido do arquivo de dados

    // dados iniciais salvos no arquivo
    novoNodo.esquerda = -1;
    novoNodo.registro = registroLido;
    novoNodo.direita = -1;

    // a posicao do novo nodo sera ao total atual
    posNovoNodo = totalNodos;

    if (posNovoNodo == 0) {
      // insere na raiz pois ainda nao ha nodos
      fwrite(&novoNodo, sizeof(Nodo), 1, arquivoArvore);
      totalNodos++;
    } else {
      // a árvore já tem nodos, precisamos achar onde inserir
      long posAtual = 0; // começa a procura pela raiz
      int inserido = 0;

      while (!inserido) {
        // lê o nodo atual para comparar a chave
        fseek(arquivoArvore, posAtual * sizeof(Nodo), SEEK_SET);
        if (fread(&nodoAtual, sizeof(Nodo), 1, arquivoArvore) != 1) {
          fclose(arquivoRegistros);
          fclose(arquivoArvore);
          return 0;
        }
        metricas.transferencias++; // nodo lido da arvore (memoria externa)

        if (COMP(registroLido.chave < nodoAtual.registro.chave)) {
          // vai para a esquerda se a chave for menor
          if (nodoAtual.esquerda == -1) {
            // achou o espaço vazio
            // atualiza o ponteiro esquerdo do pai
            nodoAtual.esquerda = posNovoNodo;
            fseek(arquivoArvore, posAtual * sizeof(Nodo), SEEK_SET);
            fwrite(&nodoAtual, sizeof(Nodo), 1, arquivoArvore);

            // grava o novo nodo lá no final do arquivo
            fseek(arquivoArvore, 0, SEEK_END);
            fwrite(&novoNodo, sizeof(Nodo), 1, arquivoArvore);
            inserido = 1;
            // incremeta o total de nodos
            totalNodos++;
          } else {
            // enquanto nao achar, continua descendo pela esquerda
            posAtual = nodoAtual.esquerda;
          }
        } else if (COMP(registroLido.chave > nodoAtual.registro.chave)) {
          // vai para a direita se a chave for maior
          if (nodoAtual.direita == -1) {
            // achou o espaço vazio
            // atualiza o ponteiro direito do pai
            nodoAtual.direita = posNovoNodo;
            fseek(arquivoArvore, posAtual * sizeof(Nodo), SEEK_SET);
            fwrite(&nodoAtual, sizeof(Nodo), 1, arquivoArvore);

            // grava o novo nodo lá no final do arquivo
            fseek(arquivoArvore, 0, SEEK_END);
            fwrite(&novoNodo, sizeof(Nodo), 1, arquivoArvore);
            inserido = 1;
            // incrementa o total de nodos
            totalNodos++;
          } else {
            // continua descendo pela direita enquanto nao achar
            posAtual = nodoAtual.direita;
          }
        } else {
          // chave repetida
          fclose(arquivoRegistros);
          fclose(arquivoArvore);
          return 0;
        }
      }
    }
  }

  fclose(arquivoRegistros);
  fclose(arquivoArvore);
  return 1;
}

int pesquisarNaArvoreBinaria(FILE *arquivoArvore, int chaveBuscada,
                             TipoRegistro *registro) {
  long posAtual = 0; // começa da raiz
  Nodo nodoAtual;

  while (posAtual != -1) {
    // pula para a posição atual e lê o nodo
    fseek(arquivoArvore, posAtual * sizeof(Nodo), SEEK_SET);
    if (fread(&nodoAtual, sizeof(Nodo), 1, arquivoArvore) != 1)
      return 0; // arvore vazia ou erro de leitura
    metricas.transferencias++; // nodo trazido para a memoria interna

    if (COMP(chaveBuscada == nodoAtual.registro.chave)) {
      *registro = nodoAtual.registro;
      return 1;
    } else if (COMP(chaveBuscada < nodoAtual.registro.chave)) {
      posAtual = nodoAtual.esquerda; // desce para a esquerda
    } else {
      posAtual = nodoAtual.direita; // desce para a direita
    }
  }
  return 0;
}
