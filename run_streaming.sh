#!/usr/bin/env bash
# Streaming launcher.
# Usage:
#   ./run_streaming.sh server   [extra args]
#   ./run_streaming.sh streamer [extra args]
#   ./run_streaming.sh receiver [extra args]

set -euo pipefail

PRESET=${STREAMING_PRESET:-ninja}
BUILD_DIR="build/${PRESET}/streaming"

case "${1:-}" in
  server)
    exec "${BUILD_DIR}/streaming_signaling_server/Release/streaming_signaling_server" "${@:2}"
    ;;
  streamer)
    LOG_DIR="benchmark_logs"
    mkdir -p "$LOG_DIR"
    COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
    BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    LOG_FILE="$(pwd)/${LOG_DIR}/${TIMESTAMP}_${COMMIT}_${BRANCH}_encode.log"
    {
      printf "# streaming pipeline stats (encode)\n"
      printf "# date:   %s\n" "$(date)"
      printf "# commit: %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
      printf "# branch: %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
      printf "# preset: %s\n" "$PRESET"
      printf "#\n"
    } > "$LOG_FILE"
    echo "Stats log: ${LOG_FILE}"
    exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" \
      --stats-log "${LOG_FILE}" "${@:2}"
    ;;
  receiver)
    LOG_DIR="benchmark_logs"
    mkdir -p "$LOG_DIR"
    COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
    BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    LOG_FILE="$(pwd)/${LOG_DIR}/${TIMESTAMP}_${COMMIT}_${BRANCH}_decode.log"
    {
      printf "# streaming pipeline stats (decode)\n"
      printf "# date:   %s\n" "$(date)"
      printf "# commit: %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
      printf "# branch: %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
      printf "# preset: %s\n" "$PRESET"
      printf "#\n"
    } > "$LOG_FILE"
    echo "Stats log: ${LOG_FILE}"
    exec "${BUILD_DIR}/streaming_receiver/Release/streaming_receiver" \
      --stats-log "${LOG_FILE}" "${@:2}"
    ;;
  *)
    echo "Usage: $0 {server|streamer|receiver} [extra args]"
    exit 1
    ;;
esac
