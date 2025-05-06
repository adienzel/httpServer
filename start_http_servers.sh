#!/bin/bash

# Defaults
DEFAULT_NUM_CONTAINERS=5
DEFAULT_START_PORT=8082
IMAGE_NAME="http-server"
NAME_PREFIX="http_server_"

# CLI args
NUM_CONTAINERS=${1:-$DEFAULT_NUM_CONTAINERS}
START_PORT=${2:-$DEFAULT_START_PORT}

echo "Launching $NUM_CONTAINERS container(s) from '$IMAGE_NAME' using --network host..."
echo "Base port: $START_PORT (via HTTP_SERVER_PORT_PORT env var)"

for i in $(seq 0 $((NUM_CONTAINERS - 1))); do
  CONTAINER_NAME="${NAME_PREFIX}${i}"
  PORT=$((START_PORT + i))

  echo "  -> Starting container $CONTAINER_NAME with HTTP_SERVER_PORT_PORT=$PORT"

  docker run -d \
    --network host \
    --name ${CONTAINER_NAME} \
    -e HTTP_SERVER_PORT_PORT=${PORT} \
    ${IMAGE_NAME}
done
