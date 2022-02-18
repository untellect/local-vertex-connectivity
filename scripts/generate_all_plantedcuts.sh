#!/bin/bash

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd ${SCRIPTPATH%scripts*}

#
# Generate all directed planted cuts
#

# Variable (n, m) with constant average degree
scripts/generators/cpc/cpcgen_varn.sh 5 4 5 8  5 50 500 50             # kappa=4, small
scripts/generators/cpc/cpcgen_varn.sh 5 8 9 16 5 100 2000 100          # kappa=8, small
scripts/generators/cpc/cpcgen_varn.sh 5 4 5 8  5 10000 50000 10000     # kappa=4, medium
scripts/generators/cpc/cpcgen_varn.sh 5 8 9 16 5 10000 50000 10000     # kappa=8, medium
scripts/generators/cpc/cpcgen_varn.sh 5 4 5 8  5 200000 1000000 200000 # kappa=4, large
scripts/generators/cpc/cpcgen_varn.sh 5 8 9 16 5 200000 1000000 200000 # kappa=8, large

# Variable planted cut size (up to 32)
scripts/generators/cpc/cpcgen_vars.sh 5 10000 33 64 5 1 32

# Unbalanced vs balanced planted cuts
scripts/generators/cpc/cpcgen_varl.sh 10000 4 5 8 5 5

# Variable m with constant n
scripts/generators/cpc/cpcgen_vard.sh 5 4 10000 5 8 5 5   # kappa=4, medium
scripts/generators/cpc/cpcgen_vard.sh 5 8 10000 9 16 5 5  # kappa=8, medium
scripts/generators/cpc/cpcgen_vard.sh 5 4 200000 5 8 5 5  # kappa=4, large
scripts/generators/cpc/cpcgen_vard.sh 5 8 200000 9 16 5 5 # kappa=8, large

#
# Generate all undirected planted cuts
#

# Variable n
scripts/generators/pc/pcgen_varn.sh 5 4 64 5 50 500 50             # kappa=4, small
scripts/generators/pc/pcgen_varn.sh 5 8 64 5 50 500 50             # kappa=8, small
scripts/generators/pc/pcgen_varn.sh 5 4 64 5 10000 50000 10000     # kappa=4, medium
scripts/generators/pc/pcgen_varn.sh 5 8 64 5 10000 50000 10000     # kappa=8, medium
scripts/generators/pc/pcgen_varn.sh 5 4 64 5 200000 1000000 200000 # kappa=4, large
scripts/generators/pc/pcgen_varn.sh 5 8 64 5 200000 1000000 200000 # kappa=8, large

# Variable planted cut size (up to 32)
scripts/generators/pc/pcgen_vars.sh 5 10000 64 5 1 32 33 # The generator is also used for exponentially increasing |S|. With a sufficiently large (>32) value for the last input value we get all values from 1 to 32.

# Unbalanced vs balanced planted cuts
scripts/generators/pc/pcgen_varl.sh 10000 4 64 5 10
