#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: pcgen_vark.sh L S N SAMPLE k_min k_max k_exp_factor
# Multiplies k by (k_exp_factor + 1)/k_exp_factor every time (rounded down)

if [[ -e bin/PCGEN.exe ]]; then
	GEN=bin/PCGEN.exe
else
	GEN=bin/PCGEN
fi

L=$1
S=$2
N=$3
let "R = $N - $L - $S"
SAMPLE=$4

SUITEDIR=input/PC/n${N}-k${S}-f-${5}-to-${6}-by-${7}/

for ((k = $5; $k <= $6; k = (k*($7+1) + $7-1)/$7 )); do
	PARAMDIR=${SUITEDIR}/${L}-${S}-${R}-${k}/
	mkdir -p ${PARAMDIR}
	for ((i = 0; $i < $SAMPLE; i++)); do
		$GEN $L $S $R $k 1> ${PARAMDIR}/${i}.txt 2> /dev/null
	done
done


