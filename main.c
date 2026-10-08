#include "acesso sequencial indexado/asi.h"
#include "arvore B/avB.h"
#include "arvore Estrela/aE.h"
#include "gerador bin/gerador.h"
#include "metricas.h"
#include "pesquisa binaria/pB.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Tudo o que o indice de cada metodo precisa manter em memoria interna */
typedef struct {
  int metodo;
  FILE *arquivo;               /* metodo 1: arquivo de registros aberto */
  TipoIndice *tabela;          /* metodo 1: tabela de indice das paginas */
  int paginas;                 /* metodo 1: quantidade de paginas */
  FILE *arvoreBinaria;         /* metodo 2: arquivo da arvore aberto */
  TipoApontador raizB;         /* metodo 3: raiz da arvore B (RAM) */
  TipoApontadorEstrela raizE;  /* metodo 4: raiz da arvore B* (RAM) */
} Contexto;

/* ---------------------------------------------------------------------- */
/* utilitarios                                                            */
/* ---------------------------------------------------------------------- */

/* converte texto em inteiro validando o formato. Retorna 1 se ok. */
static int lerInteiro(const char *texto, long *valor) {
  char *fim;
  errno = 0;
  *valor = strtol(texto, &fim, 10);
  return errno == 0 && fim != texto && *fim == '\0';
}

/* retorna 1 se o arquivo existe e tem exatamente 'quantidade' registros */
static int arquivoValido(char *nomeSalvo, long quantidade) {
  FILE *arquivo = fopen(nomeSalvo, "rb");
  if (arquivo == NULL)
    return 0;
  fseek(arquivo, 0, SEEK_END);
  long tamanho = ftell(arquivo);
  fclose(arquivo);
  return tamanho == quantidade * (long)sizeof(TipoRegistro);
}

/* opcao -P: imprime todas as chaves do arquivo (fora de qualquer medicao) */
static void imprimirChaves(char *nomeArquivo) {
  FILE *arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL)
    return;
  TipoRegistro *registro = malloc(sizeof(TipoRegistro));
  if (registro == NULL) {
    fclose(arquivo);
    return;
  }
  printf("Chaves do arquivo:\n");
  while (fread(registro, sizeof(TipoRegistro), 1, arquivo) == 1)
    printf(" %d ", registro->chave);
  printf("\n");
  free(registro);
  fclose(arquivo);
}

static void imprimirMetricas(const char *titulo, Metricas m) {
  printf("--- %s ---\n", titulo);
  printf("Transferencias (leituras): %ld\n", m.transferencias);
  printf("Comparacoes entre chaves : %ld\n", m.comparacoes);
  printf("Tempo de execucao        : %.9f s\n", m.tempo);
}

/* ---------------------------------------------------------------------- */
/* criacao do indice, pesquisa e liberacao (uma por metodo)               */
/* ---------------------------------------------------------------------- */

/* Constroi o indice do metodo escolhido. Retorna 1 se ok.
   E ESTA funcao (inteira) que e cronometrada como "criacao do indice". */
static int criarIndice(Contexto *ctx, char *nomeArquivo, char *nomeArvore) {
  TipoRegistro registro;
  switch (ctx->metodo) {
  case 1:
    ctx->arquivo = fopen(nomeArquivo, "rb");
    if (ctx->arquivo == NULL)
      return 0;
    ctx->tabela = malloc(sizeof(TipoIndice) * MAX_PAGINAS);
    if (ctx->tabela == NULL)
      return 0;
    return criarIndiceASI(ctx->arquivo, ctx->tabela, &ctx->paginas);

  case 2:
    if (!construirArvoreBinaria(nomeArquivo, nomeArvore))
      return 0;
    ctx->arvoreBinaria = fopen(nomeArvore, "rb");
    return ctx->arvoreBinaria != NULL;

  case 3: {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL)
      return 0;
    inicializarArvoreB(&ctx->raizB);
    while (fread(&registro, sizeof(TipoRegistro), 1, arquivo) == 1) {
      metricas.transferencias++; /* registro lido do arquivo de dados */
      inserirNaArvoreB(registro, &ctx->raizB);
    }
    fclose(arquivo);
    return 1;
  }

  case 4: {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL)
      return 0;
    inicializarAB(&ctx->raizE);
    while (fread(&registro, sizeof(TipoRegistro), 1, arquivo) == 1) {
      metricas.transferencias++; /* registro lido do arquivo de dados */
      InserirAB(registro, &ctx->raizE);
    }
    fclose(arquivo);
    return 1;
  }
  }
  return 0;
}

/* Pesquisa uma chave. Retorna 1 se achou (e preenche *registro).
   Nao imprime nada, para nao entrar no tempo medido. */
static int buscar(Contexto *ctx, int chave, TipoRegistro *registro) {
  registro->chave = chave;
  switch (ctx->metodo) {
  case 1:
    return pesquisarRegistro(ctx->tabela, ctx->paginas, registro, ctx->arquivo);
  case 2:
    return pesquisarNaArvoreBinaria(ctx->arvoreBinaria, chave, registro);
  case 3:
    return pesquisarNaArvoreB(registro, ctx->raizB);
  case 4:
    return pesquisarNaArvoreBEstrela(registro, ctx->raizE);
  }
  return 0;
}

static void liberarIndice(Contexto *ctx) {
  if (ctx->arquivo != NULL)
    fclose(ctx->arquivo);
  if (ctx->arvoreBinaria != NULL)
    fclose(ctx->arvoreBinaria);
  free(ctx->tabela);
  /* as arvores B e B* so existem se foram criadas: raiz NULL e inofensivo */
  liberarAB(&ctx->raizE);
  /* (a arvore B nao possui funcao de liberacao; o SO recupera a memoria
     ao final do processo) */
}

/* ---------------------------------------------------------------------- */
/* modos de execucao                                                      */
/* ---------------------------------------------------------------------- */

/* Modo normal: pesquisa uma unica chave e mostra resultado + metricas */
static int executarUmaChave(Contexto *ctx, char *nomeArquivo, char *nomeArvore,
                            int chave) {
  Metricas criacao, pesquisa;
  TipoRegistro registro;

  /* ---- CRIACAO DO INDICE (cronometrada) ---- */
  metricas = (Metricas){0, 0, 0.0};
  double inicio = agora();
  int ok = criarIndice(ctx, nomeArquivo, nomeArvore);
  double fim = agora();
  criacao = metricas;
  criacao.tempo = fim - inicio;
  if (!ok) {
    printf("Erro ao criar o indice do metodo %d.\n", ctx->metodo);
    return 0;
  }

  /* ---- PESQUISA (cronometrada) ---- */
  metricas = (Metricas){0, 0, 0.0};
  inicio = agora();
  int achou = buscar(ctx, chave, &registro);
  fim = agora();
  pesquisa = metricas;
  pesquisa.tempo = fim - inicio;

  /* ---- a partir daqui so impressao (fora das medicoes) ---- */
  if (achou) {
    printf("Chave %d ENCONTRADA\n", chave);
    printf("dado1 = %ld\n", registro.dado1);
    printf("dado2 = %.5000s\n", registro.dado2);
  } else {
    printf("Chave %d NAO encontrada\n", chave);
  }
  imprimirMetricas("Criacao do indice", criacao);
  imprimirMetricas("Pesquisa", pesquisa);
  return 1;
}

int main(int argc, char *argv[]) {
  long metodo, quantidade, situacao, chave = 0;
  int mostrarChaves = 0;

  if (argc < 5 || argc > 6) {
    printf("Uso: %s <metodo> <quantidade> <situacao> <chave> [-P]\n", argv[0]);
    printf("     %s <metodo> <quantidade> <situacao> -E [-P]\n", argv[0]);
    printf("  metodo    : 1 (acesso sequencial indexado), 2 (arvore binaria),\n"
           "              3 (arvore B), 4 (arvore B*)\n");
    printf("  situacao  : 1 (ascendente), 2 (descendente), 3 (aleatorio)\n");
    printf("  -P        : imprime as chaves do arquivo\n");
    return 1;
  }
  if (!lerInteiro(argv[1], &metodo) || metodo < 1 || metodo > 4) {
    printf("Erro: metodo invalido. Deve ser um numero entre 1 e 4.\n");
    return 1;
  }
  if (!lerInteiro(argv[2], &quantidade) || quantidade <= 0) {
    printf("Erro: quantidade deve ser um inteiro maior que zero.\n");
    return 1;
  }
  if (!lerInteiro(argv[3], &situacao) || situacao < 1 || situacao > 3) {
    printf("Erro: situacao invalida. Deve ser 1, 2 ou 3.\n");
    return 1;
  }
 
  if (argc == 5) {
    if (strcmp(argv[5], "-P") != 0) {
      printf("Erro: argumento opcional invalido '%s' (use -P).\n", argv[5]);
      return 1;
    }
    mostrarChaves = 1;
  }
  /* o acesso sequencial indexado exige arquivo ordenado ascendentemente */
  if (metodo == 1 && situacao != 1) {
    printf("Erro: o metodo 1 so funciona com arquivo ascendente (situacao 1).\n");
    return 1;
  }
  if (metodo == 2 && situacao != 3 && quantidade >= 20000)
    fprintf(stderr, "Aviso: arvore binaria com arquivo ordenado degenera em "
                    "lista; a construcao pode ser extremamente lenta.\n");

  /* nome dos arquivos depende dos parametros: o de registros so e gerado se
     ainda nao existe. A geracao NAO entra em nenhuma medicao. */
  char nomeArquivo[64], nomeArvore[64];
  snprintf(nomeArquivo, sizeof(nomeArquivo), "registros_%ld_%ld.bin",
           quantidade, situacao);
  snprintf(nomeArvore, sizeof(nomeArvore), "arvorebinaria_%ld_%ld.bin",
           quantidade, situacao);

  if (!arquivoValido(nomeArquivo, quantidade)) {
    if (gerarArquivo(nomeArquivo, quantidade, (int)situacao, 42) != 0) {
      printf("Erro na criacao do arquivo de registros.\n");
      return 1;
    }
  }

  if (mostrarChaves)
    imprimirChaves(nomeArquivo);

  /* descarta o cache do SO para que todos os metodos partam "a frio"
     (fora das medicoes) */
  limparCacheArquivo(nomeArquivo);

  Contexto ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.metodo = (int)metodo;

  int ok;
 
  ok = executarUmaChave(&ctx, nomeArquivo, nomeArvore, (int)chave);

  liberarIndice(&ctx);
  return ok ? 0 : 1;
}
