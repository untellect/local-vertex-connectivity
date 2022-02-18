#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: cpcgen_vard.sh L S N k_min d_avg_min SAMPLE max_factor

if [[ -e bin/CPCGEN.exe ]]; then
	GEN=bin/CPCGEN.exe
else
	GEN=bin/CPCGEN
fi

L=$1
S=$2
N=$3
let "R = $N - $L - $S"
k_min=$4
d_avg_min=$5
SAMPLE=$6
max_factor=$7

SUITEDIR=input/CPC/k${S}-n${N}-d-to-$(($d_avg_min * $max_factor))

for ((factor = 1; $factor <= ${max_factor}; factor += 1 )); do
	let "k = $k_min * $factor"
	let "d_avg = $d_avg_min * $factor"
	PARAMSTR=${L}-${S}-${R}-${k}-${d_avg}/

	mkdir -p ${SUITEDIR}/in/${PARAMSTR}
	for ((i = 0; $i < ${SAMPLE}; i = i + 1)); do
		$GEN $L $S $R 0 $k $d_avg 1> ${SUITEDIR}/in/${PARAMSTR}/$i.txt 2> ${SUITEDIR}/in/${PARAMSTR}/$i.meta
	done
	mkdir -p ${SUITEDIR}/out/${PARAMSTR}
	for ((i = 0; $i < ${SAMPLE}; i = i + 1)); do
		$GEN $L $R $S 0 $k $d_avg 1> ${SUITEDIR}/out/${PARAMSTR}/$i.txt 2> ${SUITEDIR}/out/${PARAMSTR}/$i.meta
	done
done
