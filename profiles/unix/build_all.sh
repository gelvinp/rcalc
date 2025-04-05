#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/../.." # Root level directory

platform=""

if [ "$#" -eq 1 ]; then
    platform="platform=$1"
fi

for profile in profiles/unix/*.py; do
    scons profile=$profile $platform
done