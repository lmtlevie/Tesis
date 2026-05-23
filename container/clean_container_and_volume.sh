#!/usr/bin/env bash

# Default values
CONTAINER_NAME="pd-container"
VOLUME_NAME="powerdevs-volume"

# Parse input arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --container-name)
        CONTAINER_NAME="$2"
        shift # Remove current argument
        shift # Remove current value
        ;;
        --volume-name)
        VOLUME_NAME="$2"
        shift # Remove current argument
        shift # Remove current value
        ;;
        *)
        # If the argument is unknown, shift and continue
        shift
        ;;
    esac
done


echo "Attempting to stop Docker container named $CONTAINER_NAME..."
docker stop $CONTAINER_NAME
if [ $? -ne 0 ]; then
    echo "Warning: Error stopping the docker container. Continuing..."
fi

echo "Attempting to remove Docker container named $CONTAINER_NAME..."
docker rm $CONTAINER_NAME
if [ $? -ne 0 ]; then
    echo "Warning: Error removing the docker container. Continuing..."
fi

echo "Attempting to remove Docker volume named $VOLUME_NAME..."
docker volume rm $VOLUME_NAME
if [ $? -ne 0 ]; then
    echo "Error removing docker volume."
    exit 1
fi
