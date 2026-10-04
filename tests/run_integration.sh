#!/bin/sh
# Integration smoke test: run the built binary against the fixture root.
# usage: run_integration.sh <procpulse-binary> <fixtures-root>
set -e

BIN=$1
FIX=$2

OUT=$("$BIN" --root "$FIX" --once)

echo "$OUT" | grep -q "usage over sample window: 0.0 %" || { echo "INTEGRATION FAIL: cpu usage"; exit 1; }
echo "$OUT" | grep -q "load average: 0.5 0.6 0.6"        || { echo "INTEGRATION FAIL: loadavg"; exit 1; }
echo "$OUT" | grep -q "usage: 60.0 %"                   || { echo "INTEGRATION FAIL: memory"; exit 1; }
echo "$OUT" | grep -q "Processes (2)"                   || { echo "INTEGRATION FAIL: processes"; exit 1; }
echo "$OUT" | grep -q "Devices (3"                      || { echo "INTEGRATION FAIL: devices"; exit 1; }
echo "$OUT" | grep -q "device nodes in /dev: 2"         || { echo "INTEGRATION FAIL: dev nodes"; exit 1; }

"$BIN" --root "$FIX" --live 1 --interval 0.3 | grep -q "sampler stopped cleanly" \
    || { echo "INTEGRATION FAIL: live mode"; exit 1; }

echo "ALL INTEGRATION CHECKS PASSED"
