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

typedef struct {
  int metodo;
  FILE *arquivo;
  TipoIndice *tabela;
  int paginas;
  FILE *arvoreBinaria;
  TipoApontador raizB;
  TipoApontadorEstrela raizE;
} Contexto;

static int arquivoValido(char *nomeSalvo, long quantidade) {
  FILE *arquivo = fopen(nomeSalvo, "rb");
  if (arquivo == NULL)
    return 0;
  fseek(arquivo, 0, SEEK_END);
  long tamanho = ftell(arquivo);
  fclose(arquivo);
  return tamanho == quantidade * (long)sizeof(TipoRegistro);
}

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
      metricas.transferencias++;
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
      metricas.transferencias++;
      InserirAB(registro, &ctx->raizE);
    }
    fclose(arquivo);
    return 1;
  }
  }
  return 0;
}

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
  liberarAB(&ctx->raizE);
}

static int executarUmaChave(Contexto *ctx, char *nomeArquivo, char *nomeArvore,
                            int chave) {
  Metricas criacao, pesquisa;
  TipoRegistro registro;

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

  metricas = (Metricas){0, 0, 0.0};
  inicio = agora();
  int achou = buscar(ctx, chave, &registro);
  fim = agora();
  pesquisa = metricas;
  pesquisa.tempo = fim - inicio;

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
    printf("     %s <metodo> <quantidade> <situacao> [-P]\n", argv[0]);
    printf("  metodo    : 1 (acesso sequencial indexado), 2 (arvore binaria),\n"
           "              3 (arvore B), 4 (arvore B*)\n");
    printf("  situacao  : 1 (ascendente), 2 (descendente), 3 (aleatorio)\n");
    printf("  -P        : imprime as chaves do arquivo\n");
    return 1;
  }
  
  metodo = atoi(argv[1]);
  if (metodo < 1 || metodo > 4) {
    printf("Erro: metodo invalido. Deve ser um numero entre 1 e 4.\n");
    return 1;
  }
  
  quantidade = atoi(argv[2]);
  if (quantidade <= 0) {
    printf("Erro: quantidade deve ser um inteiro maior que zero.\n");
    return 1;
  }
  
  situacao = atoi(argv[3]);
  if (situacao < 1 || situacao > 3) {
    printf("Erro: situacao invalida. Deve ser 1, 2 ou 3.\n");
    return 1;
  }
  
  chave = atoi(argv[4]);

  if (argc == 6) {
    if (strcmp(argv[5], "-P") != 0) {
      printf("Erro: argumento opcional invalido '%s' (use -P).\n", argv[5]);
      return 1;
    }
    mostrarChaves = 1;
  }
  
  if (metodo == 1 && situacao != 1) {
    printf("Erro: o metodo 1 so funciona com arquivo ascendente (situacao 1).\n");
    return 1;
  }
  
  if (metodo == 2 && situacao != 3 && quantidade >= 20000)
    fprintf(stderr, "Aviso: arvore binaria com arquivo ordenado degenera em "
                    "lista; a construcao pode ser extremamente lenta.\n");

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

  limparCacheArquivo(nomeArquivo);

  Contexto ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.metodo = (int)metodo;

  int ok;

  ok = executarUmaChave(&ctx, nomeArquivo, nomeArvore, (int)chave);

  liberarIndice(&ctx);
  return ok ? 0 : 1;
}