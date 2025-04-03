#!/bin/bash

set -e

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/.." # Root level directory

find . -name "*.gcda" -delete

scons use_coverage=yes

bin/$(ls -t bin | head -n1) --run-tests --filter "*"

set +e

lcov --capture --directory app --directory core --directory tests --ignore-errors inconsistent --output-file coverage.info
lcov -r coverage.info "Applications" -ignore-errors inconsistent -ignore-errors unused -output-file coverage.info
lcov -r coverage.info "/usr/include/c++" -ignore-errors inconsistent -ignore-errors unused -output-file coverage.info
lcov -r coverage.info "modules" -ignore-errors inconsistent -output-file coverage.info
lcov -r coverage.info "tests" -ignore-errors inconsistent -output-file coverage.info
lcov -r coverage.info "main" -ignore-errors inconsistent -output-file coverage.info
genhtml coverage.info -ignore-errors inconsistent --output-directory coverage.gen