#ifndef METRICAS_H
#define METRICAS_H

// metricas de uma fase (criacao do indice ou pesquisa)
typedef struct {
  long transferencias; // registros/nodos lidos da memoria externa
  long comparacoes;    // comparacoes entre chaves de pesquisa
  double tempo;        // segundos
} Metricas;

// contadores da fase em andamento (definida em metricas.c)
extern Metricas metricas;

// tempo atual em segundos (relogio monotonico, resolucao de ns)
double agora(void);

// pede ao sistema para descartar o cache do arquivo (execucao "a frio")
void limparCacheArquivo(const char *nomeArquivo);

// conta uma comparacao de chaves e devolve o resultado da propria comparacao
#define COMP(expr) (metricas.comparacoes++, (expr))

#endif
