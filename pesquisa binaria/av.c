#include <stdio.h>
#include <stdlib.h>
#include "asi.h"


#define ITENSPAGINA 3
#define MAXPAGINAS 2000000

typedef struct{
    TipoRegistro algo;
    int esq = -1;
    int dir = -1;
}Nodo;

int arvoreBinaria(){
    FILE* arq, bin;
    Nodo x;
    arq = fopen("NomedoGerador", "w");
    bin = fopen("Arvore.bin", "wb");
    fread(&x, sizeof(x), 1, arq);
    frwite(&x, sizeof(x), 1, bin);
    while(fread(&x, sizeof(x), 1, arq) == 1){
       fwrite(&x, sizeof(x), 1, bin);
    }
}