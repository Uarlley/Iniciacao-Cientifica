#!/bin/bash

# nohup ./exec.sh >> Saida.txt &

FLAGS=""
FLAGS+=" -tl 7200 "
FLAGS+=" -logCplex "
FLAGS+=" -preSolve "
FLAGS+=" -mem 16000 "
FLAGS+=" -threads 1 "

metodo="2"
diretorioInstancia=./Instancias/instancia
diretorioSaida=./Saidas/$metodo

echo ""
echo "Tempo limite por iteração = 2 horas"
echo "$metodo"

for(( constante=1; constante<=5; constante++ ))
do
   echo "$constante"
   for(( inst=1; inst<=30; inst++ ))
   do
     #echo "instancia = $inst"
     ./Metodos -i $diretorioInstancia$inst.txt -modelo $metodo -const $constante $FLAGS >$diretorioSaida/$inst-$metodo.txt
   done
   #echo "\\----------------------------------------------------------\\"
done


