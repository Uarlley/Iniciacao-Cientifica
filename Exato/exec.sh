#!/bin/bash

# nohup ./exec.sh >> Saida.txt &

FLAGS=""
FLAGS+=" -tl 7200 "
FLAGS+=" -logCplex "
FLAGS+=" -preSolve "
FLAGS+=" -mem 16000 "
FLAGS+=" -threads 1 "

metodo="1"

diretorioInstancia=./Instancias/instancia
diretorioSaida=./Saidas

echo ""
echo "Tempo limite por iteração = 2 horas"

for(( inst=1; inst<=30; inst++ ))
do
   ./Exato -i $diretorioInstancia$inst.txt $FLAGS >$diretorioSaida/$inst.txt
done


