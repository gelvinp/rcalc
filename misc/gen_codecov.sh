#!/bin/bash

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/.." # Root level directory

lcov --capture --directory app --directory core --directory tests --ignore-errors inconsistent --output-file coverage.info
genhtml coverage.info -ignore-errors inconsistent --output-directory coverage.gen

firefox coverage.gen/index.html