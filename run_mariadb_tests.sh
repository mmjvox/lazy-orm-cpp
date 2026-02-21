#!/bin/bash

# Script to run all mariadb tests for lazy-orm

cd "$(dirname "$0")/build/tests"

echo "Running all mariadb tests..."
echo "============================"

ctest --output-on-failure

echo "============================"
echo "Tests completed."