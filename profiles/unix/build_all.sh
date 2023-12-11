#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/../.." # Root level directory

for profile in profiles/unix/*.py; do
    scons profile=$profile
done