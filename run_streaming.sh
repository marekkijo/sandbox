#!/usr/bin/env bash
# Streaming runner.
# Starts signaling server, streamer, and receiver.
# When STREAMING_PIPELINE_STATS is enabled (default), pipeline stats are written
# to a timestamped log file in benchmark_logs/ instead of stdout.
#
# Usage:
#   ./run_streaming.sh
#   STREAMING_PRESET=ninja_debug ./run_streaming.sh

set -uo pipefail

PRESET=${STREAMING_PRESET:-ninja}

BUILD_DIR="build/${PRESET}/streaming"
SERVER="${BUILD_DIR}/streaming_signaling_server/Release/streaming_signaling_server"
STREAMER="${BUILD_DIR}/streaming_streamer/Release/streaming_streamer"
RECEIVER="${BUILD_DIR}/streaming_receiver/Release/streaming_receiver"

for bin in "$SERVER" "$STREAMER" "$RECEIVER"; do
  if [[ ! -x "$bin" ]]; then
    echo "Binary not found: $bin"
    echo "Build first:  cmake --build --preset=${PRESET}"
    exit 1
  fi
done

# --- Log file for pipeline stats ---
LOG_DIR="benchmark_logs"
mkdir -p "$LOG_DIR"
COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_FILE="${LOG_DIR}/${TIMESTAMP}_${COMMIT}_${BRANCH}.log"

{
  printf "# streaming pipeline stats\n"
  printf "# date:   %s\n" "$(date)"
  printf "# commit: %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
  printf "# branch: %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
  printf "# preset: %s\n" "$PRESET"
  printf "#\n"
} > "$LOG_FILE"

echo "Stats log: ${LOG_FILE}"
echo

# --- Cleanup ---
SERVER_PID="" STREAMER_PID="" RECEIVER_PID=""
cleanup() {
  kill "$SERVER_PID" "$STREAMER_PID" "$RECEIVER_PID" 2>/dev/null || true
  wait 2>/dev/null || true
  echo ""
  echo "Log: ${LOG_FILE}"
}
trap cleanup EXIT INT TERM

# --- Launch ---
"${SERVER}" 2>&1 | sed 's/^/[server  ] /' &
SERVER_PID=$!
sleep 0.5

"${STREAMER}" 2>&1 | sed 's/^/[streamer] /' &
STREAMER_PID=$!

"${RECEIVER}" --stats-log "${LOG_FILE}" 2>&1 | sed 's/^/[receiver] /' &
RECEIVER_PID=$!

echo "All processes started. Press Ctrl-C to stop."
wait "$RECEIVER_PID"
