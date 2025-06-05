#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

# Build aflat benchmark
../bin/aflat build >/dev/null

# Run and time aflat binary
{ time ./bin/sum_bench >/dev/null; } 2>aflat_time.txt

# Run and time python
{ time python3 sum.py >/dev/null; } 2>python_time.txt

# Run and time node
{ time node sum.js >/dev/null; } 2>node_time.txt

echo "AFlat $(grep real aflat_time.txt | awk '{print $2}')"
echo "Python $(grep real python_time.txt | awk '{print $2}')"
echo "Node $(grep real node_time.txt | awk '{print $2}')"
