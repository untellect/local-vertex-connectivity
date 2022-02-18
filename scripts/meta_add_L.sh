#!/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

for PARAMDIR in data/current/${1}/*/; do
	echo $PARAMDIR
	PARAMS=$(basename $PARAMDIR)
	L=${PARAMS%%-*} # Assumes L-S-R-k format
	S=${PARAMS#${L}-}
	S=${S%%-*}
	for META in $PARAMDIR*/[ud]META.txt; do # Does nothing if META.txt does not already exist.
		if [[ "$(basename $META)" == "dMETA.txt" ]]; then
			INPUT=${META#data/current/}
			INPUT=${INPUT%/dMETA.txt}
			bin/extra/dGETVOL input/$INPUT >> $META
		elif [[ "$(basename $META)" == "uMETA.txt" ]]; then
			INPUT=${META#data/current/}
			INPUT=${INPUT%/uMETA.txt}.txt
			bin/extra/uGETVOL $(($S + 1)) input/$INPUT >> $META
		else
			echo "L	"$L >> $META
		fi
	done
done
