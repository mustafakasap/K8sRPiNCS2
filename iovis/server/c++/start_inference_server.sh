#!/bin/bash

echo "Setup env vars"
source $INTEL_OPENVINO_DIR/bin/setupvars.sh

echo "Starting inference server from start.sh"
./inference_server
