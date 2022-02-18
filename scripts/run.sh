#!/bin/bash
# Usage: run.sh BIN SAMPLESIZE DIRECTORY/FILE

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

BIN=bin/${1}
BINFILE=$1
SAMPLE=$2
CURRENT=$3
DEADLINE=${SAMPLE}h # 1h on average

if ! [[ -e ${BIN} ]]; then
	echo "Not found: "${BIN}
	return
fi

if [[ -d input/${CURRENT} ]]; then
	for SUB in input/${CURRENT}/*; do
		$0 $1 $2 ${SUB#input/}
	done
elif [[ $(basename input/$CURRENT) == *.txt ]]; then
	echo ${CURRENT}
	mkdir -p data/current/${CURRENT%.*}
	timeout ${DEADLINE} ${BIN} ${SAMPLE} input/${CURRENT} > data/current/${CURRENT%.*}/${BINFILE%.*}.txt
fi
