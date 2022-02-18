#!/usr/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

if [[ -z $1 ]]; then
	$0 data/current
	exit 0
fi

depth=$(find $1 -type d -printf '%d\n' | sort -rn | head -1)

if [[ $depth > 2 ]]; then
	for x in $1/*; do
		$0 $x
	done
elif [[ $depth == 2 ]]; then
	mkdir -p collected-data
	x=${1#data/current/}
	echo $x
	${0%/*}/collect.py $x
	cp $1/results.csv collected-data/$(echo $x | tr / _).csv
	if [[ "$x" == *"-L5"* ]] || [[ "$x" == *"-L10"* ]]; then
		echo $x-norm
		${0%/*}/collect-normalised.py $x
		cp $1/norm-results.csv collected-data/norm_$(echo $x | tr / _).csv
	fi
fi
