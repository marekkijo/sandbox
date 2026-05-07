#!/usr/bin/env bash
# Streaming benchmark runner.
# Runs signaling server, streamer, and receiver for a fixed duration,
# collecting pipeline timing-stat reports and saving everything to a log file.
#
# Usage:
#   ./run_streaming.sh
#   REPORTS=30 ./run_streaming.sh
#   FPS=60 STREAMING_PRESET=ninja_debug ./run_streaming.sh

set -uo pipefail

PRESET=${STREAMING_PRESET:-ninja}
REPORTS=${REPORTS:-20}
REPORT_INTERVAL=100  # must match PIPELINE_STATS_REPORT_INTERVAL in pipeline_stats.hpp
FPS=${FPS:-30}

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

# --- Log file ---
LOG_DIR="benchmark_logs"
mkdir -p "$LOG_DIR"
COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_FILE="${LOG_DIR}/${TIMESTAMP}_${COMMIT}_${BRANCH}.log"

# Duration: enough frames for all requested reports + startup buffer
DURATION=$(( REPORTS * REPORT_INTERVAL / FPS + 15 ))

{
  printf "# streaming benchmark\n"
  printf "# date:    %s\n" "$(date)"
  printf "# commit:  %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
  printf "# branch:  %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
  printf "# preset:  %s\n" "$PRESET"
  printf "# target:  %d reports x %d frames @ %d fps (~%ds)\n" \
         "$REPORTS" "$REPORT_INTERVAL" "$FPS" "$DURATION"
  printf "#\n"
} > "$LOG_FILE"

echo "Streaming benchmark"
echo "  reports:  ${REPORTS} x ${REPORT_INTERVAL} frames @ ${FPS} fps"
echo "  duration: ~${DURATION}s"
echo "  log:      ${LOG_FILE}"
echo

# --- Cleanup ---
SERVER_PID="" STREAMER_PID="" RECEIVER_PID=""
cleanup() {
  kill "$SERVER_PID" "$STREAMER_PID" "$RECEIVER_PID" 2>/dev/null || true
  wait 2>/dev/null || true
  echo ""
  echo "Benchmark done. Log: ${LOG_FILE}"
}
trap cleanup EXIT INT TERM

# --- Launch (each pipeline: app → prefix → tee to terminal + log) ---
"${SERVER}"   2>&1 | sed 's/^/[server  ] /' | tee -a "$LOG_FILE" &
SERVER_PID=$!
sleep 0.5

"${STREAMER}" 2>&1 | sed 's/^/[streamer] /' | tee -a "$LOG_FILE" &
STREAMER_PID=$!

"${RECEIVER}" 2>&1 | sed 's/^/[receiver] /' | tee -a "$LOG_FILE" &
RECEIVER_PID=$!

sleep "$DURATION"
