#!/usr/bin/env bash
# Streaming runner / benchmark.
# Starts signaling server, streamer, and receiver.
# The receiver auto-closes after REPORTS stat cycles and writes stats to a
# timestamped log file in benchmark_logs/.
#
# Usage:
#   ./run_streaming.sh
#   REPORTS=10 ./run_streaming.sh
#   STREAMING_PRESET=ninja_debug ./run_streaming.sh

set -uo pipefail

PRESET=${STREAMING_PRESET:-ninja}
REPORTS=${REPORTS:-20}

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
  printf "# date:    %s\n" "$(date)"
  printf "# commit:  %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
  printf "# branch:  %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
  printf "# preset:  %s\n" "$PRESET"
  printf "# reports: %s x 100 frames\n" "$REPORTS"
  printf "#\n"
} > "$LOG_FILE"

echo "Stats log:  ${LOG_FILE}"
echo "Reports:    ${REPORTS} x 100 frames"
echo

# --- Cleanup: kill all on exit / Ctrl-C ---
SERVER_PID="" STREAMER_PID="" RECEIVER_PID=""
cleanup() {
  for pid in "$SERVER_PID" "$STREAMER_PID" "$RECEIVER_PID"; do
    if [[ -n "$pid" ]]; then
      kill "$pid" 2>/dev/null || true
    fi
  done
  wait 2>/dev/null || true
  echo ""
  echo "Done. Log: ${LOG_FILE}"
}
trap cleanup EXIT INT TERM

# Launch server — show output directly
"${SERVER}" &
SERVER_PID=$!
sleep 0.5

# Launch streamer — show output (FFmpeg warnings are useful for debugging)
"${STREAMER}" &
STREAMER_PID=$!

# Launch receiver — stats written to LOG_FILE; suppress FFmpeg noise on stderr
"${RECEIVER}" --stats-log "${LOG_FILE}" --stats-reports "${REPORTS}" 2>/dev/null &
RECEIVER_PID=$!

echo "All processes started (Ctrl-C to stop early)."
# Wait for receiver to auto-close after N reports
wait "$RECEIVER_PID" || true
