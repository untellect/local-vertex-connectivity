#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: cpcgen_varl.sh N S k d_avg SAMPLE L_exp_factor
# Multiplies L by (L_exp_factor + 1)/L_exp_factor every time (rounded down)

if [[ -e bin/CPCGEN.exe ]]; then
        GEN=bin/CPCGEN.exe
else
        GEN=bin/CPCGEN
fi

N=$1
S=$2
k=$3
d_avg=$4
SAMPLE=$5
Lfact=$6
let "Lmax = $N / 2"

SUITEDIR=input/CPC/k${S}-n${N}-L${Lfact}

for ((L = 1; $L <= ${Lmax}; L = (L*(${Lfact}+1) + ${Lfact}-1)/${Lfact} )); do
	let "R = $N - $L - $S"
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


