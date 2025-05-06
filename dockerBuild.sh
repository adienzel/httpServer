#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <image_name>"
    exit 1
fi

IMAGE_NAME=$1

# Build the Docker image
docker build -t ${IMAGE_NAME} .

# Push the Docker image to Docker Hub
#docker push ${USERNAME}/${IMAGE_NAME}
