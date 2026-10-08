#!/bin/sh
# Roda todas as combinacoes (metodo x quantidade x situacao) no modo -E
# e grava as medias em resultados.csv.
# Uso: ./experimentos.sh            (todas as quantidades)
#      ./experimentos.sh 100 2000   (apenas as quantidades listadas)
SAIDA=resultados.csv
QTDS=${*:-"100 200 2000 20000 200000 2000000"}
echo "metodo;quantidade;situacao;criacao_transf;criacao_comp;criacao_tempo_s;pesq_transf_media;pesq_comp_media;pesq_tempo_medio_s;nao_encontradas" > $SAIDA
for q in $QTDS; do
  for s in 1 2 3; do
    for m in 1 2 3 4; do
      # metodo 1 so se aplica a arquivo ascendente
      if [ $m -eq 1 ] && [ $s -ne 1 ]; then continue; fi
      echo "metodo $m, $q registros, situacao $s ..."
      ./pesquisa $m $q $s -E | grep '^CSV;' | sed 's/^CSV;//' >> $SAIDA
    done
    rm -f registros_${q}_${s}.bin arvorebinaria_${q}_${s}.bin  # libera disco
  done
done
echo "Resultados em $SAIDA"
