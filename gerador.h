#ifndef GERADOR_H
#define GERADOR_H

/*
 * gerarArquivo
 * ---------------------------------------------------------------------
 * Gera um arquivo binario de registros no formato exigido pelo TP1:
 *   - chave  : int          (4 bytes)
 *   - dado1  : long         (8 bytes)
 *   - dado2  : char[5000]   (string de ate 4999 caracteres + '\0')
 *
 * Parametros:
 *   nomeArquivo : caminho/nome do arquivo binario a ser criado
 *   quantidade  : numero de registros a gerar
 *   situacao    : 1 = ascendente, 2 = descendente, 3 = aleatoria
 *   seed        : semente do gerador pseudoaleatorio
 *
 * Retorno:
 *   0 em caso de sucesso, diferente de 0 em caso de erro
 * ---------------------------------------------------------------------
 */
int gerarArquivo(const char *nomeArquivo, long quantidade, int situacao,
                 unsigned int seed);

#endif