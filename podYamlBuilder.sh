#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <path_to_pod_yaml> <container_name> <image_name>"
    exit 1
fi

POD_YAML_PATH=$1
CONTAINER_NAME=$2
IMAGE_NAME=$3

# Update the pod.yaml file with the container name and image
cat <<EOF > ${POD_YAML_PATH}
apiVersion: v1
kind: Pod
metadata:
  name: ${CONTAINER_NAME}
spec:
  containers:
  - name: ${CONTAINER_NAME}
    image: ${IMAGE_NAME}
    ports:
    - containerPort: 8080
EOF
