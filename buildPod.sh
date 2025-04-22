#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path_to_pod_yaml>"
    exit 1
fi

POD_YAML_PATH=$1

# Apply the Kubernetes pod configuration
kubectl apply -f ${POD_YAML_PATH}
