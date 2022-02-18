#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

# Usage: cpcgen_varn.sh L S k d_avg SAMPLE N_min N_max N_increment

if [[ -e bin/CPCGEN.exe ]]; then
	GEN=bin/CPCGEN.exe
else
	GEN=bin/CPCGEN
fi

L=$1
S=$2
k=$3
d_avg=$4
SAMPLE=$5
Nmin=$6
Nmax=$7
Ninc=$8

SUITEDIR=input/CPC/k${S}-n-to-${Nmax}

for ((N = ${Nmin}; $N <= ${Nmax}; N += ${Ninc} )); do
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
