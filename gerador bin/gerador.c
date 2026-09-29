#define _POSIX_C_SOURCE 200112L
#include "gerador.h"
#include "../registro.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void preencherDado2(char *destino, size_t tamanho, int chave) {
  snprintf(destino, tamanho, "texto %d", chave);
}

static void embaralhar(int *vetor, int tamanho, unsigned int *semente) {
  for (int i = tamanho - 1; i > 0; i--) {
    int j = rand_r(semente) % (i + 1);
    int temp = vetor[i];
    vetor[i] = vetor[j];
    vetor[j] = temp;
  }
}

int gerarArquivo(const char *nomeArquivo, long quantidade, int situacao,
                 unsigned int semente) {
  if (quantidade <= 0) {
    fprintf(stderr, "Erro: quantidade deve ser um inteiro positivo.\n");
    return 1;
  }
  if (situacao != 1 && situacao != 2 && situacao != 3) {
    fprintf(stderr, "Erro: situacao deve ser 1, 2 ou 3.\n");
    return 1;
  }

  int *chaves = malloc(sizeof(int) * quantidade);
  if (!chaves) {
    fprintf(stderr, "Erro: memoria insuficiente para gerar as chaves.\n");
    return 1;
  }
  for (long i = 0; i < quantidade; i++)
    chaves[i] = (int)(i + 1);

  unsigned int sementeChaves = semente;
  if (situacao == 3) {
    embaralhar(chaves, (int)quantidade, &sementeChaves);
  } else if (situacao == 2) {
    for (long i = 0; i < quantidade / 2; i++) {
      int temp = chaves[i];
      chaves[i] = chaves[quantidade - 1 - i];
      chaves[quantidade - 1 - i] = temp;
    }
  }

  FILE *arquivo = fopen(nomeArquivo, "wb");
  if (!arquivo) {
    fprintf(stderr, "Erro ao criar o arquivo '%s'.\n", nomeArquivo);
    free(chaves);
    return 1;
  }

  unsigned int sementeDados = semente + 1;
  const long TAMANHO_LOTE = 1024;
  TipoRegistro *buffer = malloc(sizeof(TipoRegistro) * TAMANHO_LOTE);
  if (!buffer) {
    fprintf(stderr, "Erro: memoria insuficiente para buffer de escrita.\n");
    fclose(arquivo);
    free(chaves);
    return 1;
  }

  long escritos = 0;
  while (escritos < quantidade) {
    long registrosNoLote = (quantidade - escritos < TAMANHO_LOTE) ? (quantidade - escritos) : TAMANHO_LOTE;
    for (long i = 0; i < registrosNoLote; i++) {
      TipoRegistro *registro = &buffer[i];
      registro->chave = chaves[escritos + i];
      registro->dado1 = (long)rand_r(&sementeDados) * (long)rand_r(&sementeDados);
      preencherDado2(registro->dado2, sizeof(registro->dado2), registro->chave);
    }
    size_t gravados = fwrite(buffer, sizeof(TipoRegistro), registrosNoLote, arquivo);
    if ((long)gravados != registrosNoLote) {
      fprintf(stderr, "Erro de escrita no arquivo (disco cheio?).\n");
      fclose(arquivo);
      free(chaves);
      free(buffer);
      return 1;
    }
    escritos += registrosNoLote;

    fflush(arquivo);
    {
      int descritor = fileno(arquivo);
      fsync(descritor);
#ifdef POSIX_FADV_DONTNEED
      posix_fadvise(descritor, 0, 0, POSIX_FADV_DONTNEED);
#endif
    }
  }

  fclose(arquivo);
  free(chaves);
  free(buffer);
  return 0;
}