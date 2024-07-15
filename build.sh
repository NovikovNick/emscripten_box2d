#!/bin/bash

###########################################
#             1. Emscripten               #
###########################################

echo "Setup emscripten"
cd ./third_parties/emsdk

# Download and install the latest SDK tools.
./emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes .emscripten file)
./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh

cd ../../

###########################################
#           2. Build project              #
###########################################

rm -rfv ./build/{*,.*}
mkdir build
cd build

emcmake cmake ..
make