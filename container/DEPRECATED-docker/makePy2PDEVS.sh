#!/bin/bash

## there is a bug in docker in which running make with an interactive exec (docker exec -it) generates a segmentation fault
## this script runs docker exec in non-interactive mode executing make

this_dir=$(readlink -f $(dirname $0))
CONTAINER_NAME="PowerDEVS-devel-$USER"

${this_dir}/startContainer.sh

docker exec --user=$USER $CONTAINER_NAME scl enable devtoolset-6 "make -j$(nproc) py2pdevs"
#docker stop $CONTAINER_NAME


