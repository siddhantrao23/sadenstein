#! /bin/bash
set -e
BUILD_DIR="build"
BUILD_BIN="build/sadenstein"

sudo apt install -y libsdl2-dev

if [ ! -d "${BUILD_DIR}" ]; then
  mkdir "${BUILD_DIR}"
fi

cd "${BUILD_DIR}"
cmake ..
make
