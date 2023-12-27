#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/../.." # Root level directory

for profile in profiles/ios/*.py; do
    scons profile=$profile
done

cd bin

for package in lib_rcalc.ios.*.zip; do
    unzip -n $package
done

zip -q -9 -r lib_rcalc.ios.all.zip include lib LICENSE.md
rm -rf include lib LICENSE.md