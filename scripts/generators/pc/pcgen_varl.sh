#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: pcgen_varl.sh N S k SAMPLE L_exp_factor
# Multiplies L by (L_exp_factor + 1)/L_exp_factor every time (rounded down)

if [[ -e bin/PCGEN.exe ]]; then
        GEN=bin/PCGEN.exe
else
        GEN=bin/PCGEN
fi

N=$1
S=$2
k=$3
SAMPLE=$4
let "Lmax = $N / 2"

SUITEDIR=input/PC/k${S}-n$1-L${5}/

for ((L = 1; $L <= ${Lmax}; L = (L*($5+1) + $5-1)/$5 )); do
	let "R = $N - $L - $S"
	PARAMDIR=${SUITEDIR}/${L}-${S}-${R}-${k}/
	mkdir -p ${PARAMDIR}
	for ((i = 0; $i < $SAMPLE; i++)); do
		$GEN $L $S $R $k 1> ${PARAMDIR}/${i}.txt 2> /dev/null
	done
done


