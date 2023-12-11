#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/../.." # Root level directory

for profile in profiles/ios/*.py; do
    scons profile=$profile
done