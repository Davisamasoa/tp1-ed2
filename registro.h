#ifndef REGISTRO_H
#define REGISTRO_H

// definição de um item do arquivo de dados
typedef struct {
  int chave;
  long dado1;
  char dado2[5000];
} TipoRegistro;

#endif
