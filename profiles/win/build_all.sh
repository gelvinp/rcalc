#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/../.." # Root level directory

for profile in profiles/win/*.py; do
    scons profile=$profile use_mingw=yes
done