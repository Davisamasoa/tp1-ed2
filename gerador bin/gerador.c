#define _POSIX_C_SOURCE 200112L
#include "gerador.h"
#include "../registro.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void preencherDado2(char *buf, size_t tam, int chave) {
  snprintf(buf, tam, "texto %d", chave);
}

static void embaralhar(int *vet, int n, unsigned int *seedp) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand_r(seedp) % (i + 1);
    int tmp = vet[i];
    vet[i] = vet[j];
    vet[j] = tmp;
  }
}

int gerarArquivo(const char *nomeArquivo, long quantidade, int situacao,
                 unsigned int seed) {
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

  unsigned int seedChaves = seed;
  if (situacao == 3) {
    embaralhar(chaves, (int)quantidade, &seedChaves);
  } else if (situacao == 2) {
    for (long i = 0; i < quantidade / 2; i++) {
      int tmp = chaves[i];
      chaves[i] = chaves[quantidade - 1 - i];
      chaves[quantidade - 1 - i] = tmp;
    }
  }

  FILE *f = fopen(nomeArquivo, "wb");
  if (!f) {
    fprintf(stderr, "Erro ao criar o arquivo '%s'.\n", nomeArquivo);
    free(chaves);
    return 1;
  }

  unsigned int seedDados = seed + 1;
  const long LOTE = 1024;
  TipoRegistro *buffer = malloc(sizeof(TipoRegistro) * LOTE);
  if (!buffer) {
    fprintf(stderr, "Erro: memoria insuficiente para buffer de escrita.\n");
    fclose(f);
    free(chaves);
    return 1;
  }

  long escritos = 0;
  while (escritos < quantidade) {
    long lote = (quantidade - escritos < LOTE) ? (quantidade - escritos) : LOTE;
    for (long i = 0; i < lote; i++) {
      TipoRegistro *r = &buffer[i];
      r->chave = chaves[escritos + i];
      r->dado1 = (long)rand_r(&seedDados) * (long)rand_r(&seedDados);
      preencherDado2(r->dado2, sizeof(r->dado2), r->chave);
    }
    size_t gravados = fwrite(buffer, sizeof(TipoRegistro), lote, f);
    if ((long)gravados != lote) {
      fprintf(stderr, "Erro de escrita no arquivo (disco cheio?).\n");
      fclose(f);
      free(chaves);
      free(buffer);
      return 1;
    }
    escritos += lote;

    fflush(f);
    {
      int fd = fileno(f);
      fsync(fd);
#ifdef POSIX_FADV_DONTNEED
      posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
#endif
    }
  }

  fclose(f);
  free(chaves);
  free(buffer);
  return 0;
}