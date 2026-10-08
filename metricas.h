#ifndef METRICAS_H
#define METRICAS_H

/* Metricas de uma fase (criacao do indice OU pesquisa) */
typedef struct {
  long transferencias; /* registros/nodos lidos da memoria externa */
  long comparacoes;    /* comparacoes entre chaves de pesquisa */
  double tempo;        /* segundos */
} Metricas;

/* contadores da fase em andamento (definida em metricas.c) */
extern Metricas metricas;

/* tempo atual em segundos (relogio monotonico, resolucao de ns) */
double agora(void);

/* pede ao SO para descartar o cache do arquivo (execucao "a frio") */
void limparCacheArquivo(const char *nomeArquivo);

/* avalia a expressao de comparacao de chaves contando-a */
#define COMP(expr) (metricas.comparacoes++, (expr))

#endif
