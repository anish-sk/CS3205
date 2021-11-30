#!/usr/bin/env bash
nodes=$1
infile=$3
outfile="out"
hi=1
lsai=5
spfi=20
make
for ((i=0; i<$nodes; i++))
do
    bash -c "exec -a ospf_${i} ./ospf -i ${i} -f ${infile} -o ${outfile} -h ${hi} -a ${lsai} -s ${spfi} &"
done
sleep $2 
for ((i=0; i<$nodes; i++))
do
    pkill -f ospf_${i}
done
