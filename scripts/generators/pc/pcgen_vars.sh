#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: pcgen_vars.sh L N k SAMPLE S_min S_max S_exp_factor
# Multiplies S by (S_exp_factor + 1)/S_exp_factor every time (rounded down)

if [[ -e bin/PCGEN.exe ]]; then
	GEN=bin/PCGEN.exe
else
	GEN=bin/PCGEN
fi

L=$1
N=$2
k=$3
SAMPLE=$4

SUITEDIR=input/PC/n${N}-k-to-${6}-by-${7}/

for ((S = $5; $S <= $6; S = (S*($7+1) + $7-1)/$7 )); do
	let "R = $N - $L - $S"
	PARAMDIR=${SUITEDIR}/${L}-${S}-${R}-${k}/
	mkdir -p ${PARAMDIR}
	for ((i = 0; $i < $SAMPLE; i++)); do
		$GEN $L $S $R $k 1> ${PARAMDIR}/${i}.txt 2> /dev/null
	done
done

