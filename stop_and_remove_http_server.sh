#!/bin/bash

NUM_CONTAINERS=5
NAME_PREFIX="http_server_"

for i in $(seq 0 $((NUM_CONTAINERS - 1))); do
  CONTAINER_NAME="${NAME_PREFIX}${i}"
  echo "Stopping and removing $CONTAINER_NAME..."
  docker rm -f ${CONTAINER_NAME}
done
