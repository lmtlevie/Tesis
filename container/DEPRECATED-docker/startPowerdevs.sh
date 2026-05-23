#!/bin/bash

this_dir=$(readlink -f $(dirname $0))
CONTAINER_NAME="PowerDEVS-devel-$USER"

${this_dir}/startContainer.sh

docker exec -it --user=$USER $CONTAINER_NAME scl enable devtoolset-6 bash
#docker stop $CONTAINER_NAME
