# TP1 ED2 — Acesso Sequencial Indexado

Trabalho prático de Estrutura de Dados II: implementação do método de
**Acesso Sequencial Indexado (ASI)** sobre um arquivo binário de registros,
com um gerador de massa de dados para testes.

## Estrutura do projeto

| Arquivo       | Responsabilidade                                              |
|---------------|-----------------------------------------------------------------|
| `gerador.c/h` | Gera o arquivo binário `registros.bin` com registros de teste  |
| `asi.c/h`     | Implementa a indexação e a busca por acesso sequencial indexado |
| `main.c`      | Ponto de entrada: chama o gerador (opcional) e a busca          |
| `Makefile`    | Automatiza a compilação                                         |

Cada registro do arquivo tem:

```c
typedef struct {
  int  chave;
  long dado1;
  char dado2[5000];
} tipoRegistro;
```

## Requisitos

- `gcc`
- `make`

Testado em macOS/Linux (usa `fsync`/`posix_fadvise`, disponíveis em
sistemas POSIX).

## Compilando com o Makefile

Na raiz do projeto:

```sh
make
```

Isso compila `main.c`, `gerador.c` e `asi.c` e gera o executável `exe`.

Outros alvos disponíveis:

```sh
make run     # compila (se necessário) e já executa ./exe
make clean   # remove os arquivos .o e o executável
```

## Executando o projeto

### 1. Gerar o arquivo de registros

O `registros.bin` **não** é versionado (veja `.gitignore`) — é preciso
gerá-lo antes de rodar a busca. Em `main.c`, descomente a chamada de
`gerarArquivo`:

```c
int main() {
  // GERAR ARQUIVO REGISTROS.BIN
  if (gerarArquivo("registros.bin", 2000000, 1, 42) != 0) {
    printf("Erro na criação do arquivo\n");
  }

  acessoSequencial();

  return 0;
}
```

Parâmetros de `gerarArquivo(nomeArquivo, quantidade, situacao, seed)`:

- `nomeArquivo`: caminho do arquivo binário a ser criado (ex.: `"registros.bin"`)
- `quantidade`: número de registros a gerar
- `situacao`: **use `1` (ordem ascendente)**. O algoritmo de ASI em `asi.c`
  assume que o arquivo está ordenado pela chave; as opções `2`
  (descendente) e `3` (aleatória) existem no gerador mas **quebram a busca**
  se usadas diretamente, pois a tabela de índice deixa de ficar em ordem
  crescente entre páginas.
- `seed`: semente do gerador pseudoaleatório (afeta apenas `dado1`/`dado2`,
  não a ordem das chaves quando `situacao = 1`)

> `quantidade` é limitada a `MAXTABELA * ITENSPAGINA` = 20000 × 100 =
> **2.000.000** de registros (limites definidos em `asi.h`). Gerar mais que
> isso faz `acessoSequencial()` recusar a leitura do arquivo com uma
> mensagem de erro, em vez de estourar a tabela de índice.

Depois de editar `main.c`, recompile:

```sh
make
```

Rode uma vez para gerar o arquivo (pode demorar dependendo de `quantidade`,
pois `registros.bin` fica grande — ex.: 2.000.000 registros ≈ 10 GB):

```sh
./exe
```

### 2. Rodar apenas a busca

Depois que `registros.bin` já existe, comente novamente a chamada de
`gerarArquivo` em `main.c` (ou deixe como está — se o arquivo já existir,
ele será sobrescrito na próxima geração) e recompile:

```c
int main() {
  // GERAR ARQUIVO REGISTROS.BIN
  // if (gerarArquivo("registros.bin", 2000000, 1, 42) != 0) {
  //   printf("Erro na criação do arquivo\n");
  // }

  acessoSequencial();

  return 0;
}
```

```sh
make
./exe
```

O programa monta a tabela de índice (uma entrada a cada 100 registros) e
pede a chave a ser pesquisada:

```
Chave do Registro desejado: 12345
O Registro <...> (chave 12345) foi localizado
```

## Limpando o projeto

```sh
make clean          # remove binários compilados
rm -f registros.bin  # remove o arquivo de dados gerado (não é feito pelo make clean)
```
