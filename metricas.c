#define _POSIX_C_SOURCE 200112L
#include "metricas.h"
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

Metricas metricas;

double agora(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

void limparCacheArquivo(const char *nomeArquivo) {
#ifdef POSIX_FADV_DONTNEED
  FILE *arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL)
    return;
  posix_fadvise(fileno(arquivo), 0, 0, POSIX_FADV_DONTNEED);
  fclose(arquivo);
#else
  (void)nomeArquivo;
#endif
}
