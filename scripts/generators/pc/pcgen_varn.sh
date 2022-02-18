#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: pcgen_varn.sh L S k SAMPLE N_min N_max N_increment

if [[ -e bin/PCGEN.exe ]]; then
	GEN=bin/PCGEN.exe
else
	GEN=bin/PCGEN
fi

L=$1
S=$2
k=$3
SAMPLE=$4

SUITEDIR=input/PC/k${S}-n-to-${6}/

for ((N = $5; $N <= $6; N += $7 )); do
	let "R = $N - $L - $S"
	PARAMDIR=${SUITEDIR}/${L}-${S}-${R}-${k}/
	mkdir -p ${PARAMDIR}
	for ((i = 0; $i < $SAMPLE; i++)); do
		$GEN $L $S $R $k 1> ${PARAMDIR}/${i}.txt 2> /dev/null
	done
done
