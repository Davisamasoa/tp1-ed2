#define _POSIX_C_SOURCE 200112L
#include "metricas.h"
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// contadores globais usados por todos os metodos na fase que esta sendo medida
Metricas metricas;

// retorna o tempo atual em segundos; o relogio monotonico nao muda se o horario do sistema for ajustado
double agora(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  // junta os segundos e os nanossegundos em um unico valor
  return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

// pede ao sistema para descartar o arquivo do cache, para que as leituras venham do disco
void limparCacheArquivo(const char *nomeArquivo) {
  // so existe em sistemas com posix_fadvise (linux); no macOS a funcao nao faz nada
#ifdef POSIX_FADV_DONTNEED
  FILE *arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL)
    return;
  posix_fadvise(fileno(arquivo), 0, 0, POSIX_FADV_DONTNEED);
  fclose(arquivo);
#else
  // evita o aviso de parametro nao usado
  (void)nomeArquivo;
#endif
}
