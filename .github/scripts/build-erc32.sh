#!/usr/bin/env bash
set -Eeuo pipefail

cat > config.ini <<'EOF'
[sparc/erc32]
BUILD_TESTS = False
BUILD_PSXTESTS = True
BUILD_SAMPLES = True
EOF

./waf configure \
  --rtems-bsps=sparc/erc32 \
  --rtems-config=config.ini \
  --prefix="${RTEMS_PREFIX:-/opt/rtems/7}"

if [[ "${COMPILE_DATABASE:-0}" == "1" ]]; then
  bear --output build/compile_commands.json -- ./waf -j"$(nproc)"
else
  ./waf -j"$(nproc)"
fi

if [[ "${RUN_TESTS:-0}" == "1" ]]; then
  mkdir -p logs
  rtems-test \
    --log=logs/erc32-sis \
    --log-mode=all \
    --report-path=logs/erc32-sis-report \
    --report-format=json \
    --rtems-bsp=erc32-sis \
    build/sparc/erc32/testsuites | tee logs/erc32-sis-console.log

  if awk -F: '/^(Failed|Timeout|Invalid|Test too long|Wrong Version|Wrong Build|Wrong Tools):/ { gsub(/[[:space:]]/, "", $2); if ($2 + 0 > 0) failed = 1 } END { exit !failed }' logs/erc32-sis-console.log; then
    echo "RTEMS SIS tests reported failures; see logs/erc32-sis-console.log" >&2
    exit 1
  fi
fi
