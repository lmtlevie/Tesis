#!/usr/bin/env bash
cd "$(dirname "$0")"
xhost "local:$(whoami)"
CONTAINER="pd-container-display"
STATUS=$(docker inspect -f '{{.State.Status}}' "$CONTAINER" 2>/dev/null)
if [ "$STATUS" = "running" ]; then
    docker attach "$CONTAINER"
elif [ "$STATUS" = "exited" ] || [ "$STATUS" = "created" ]; then
    docker start -ai "$CONTAINER"
else
    container/setup_volume_and_run_container.sh --use-display --user-name "$USER" --image-name powerdevs-cc7:slim
fi
