#!/usr/bin/env bash
# Streaming launcher.
# Usage:
#   ./run_streaming.sh server   [extra args]
#   ./run_streaming.sh streamer [extra args]
#   ./run_streaming.sh receiver [extra args]
#
# The receiver creates a timestamped session folder in benchmark_logs/ and
# updates a "latest" symlink. The streamer writes into that same session folder
# automatically (start receiver before or at the same time as streamer).

set -euo pipefail

PRESET=${STREAMING_PRESET:-ninja}
BUILD_DIR="build/${PRESET}/streaming"
BASE_LOG_DIR="$(pwd)/benchmark_logs"

case "${1:-}" in
  server)
    exec "${BUILD_DIR}/streaming_signaling_server/Release/streaming_signaling_server" "${@:2}"
    ;;

  streamer)
    # Write into the latest session folder created by the receiver
    SESSION_DIR="${BASE_LOG_DIR}/latest"
    if [[ ! -d "$SESSION_DIR" ]]; then
      echo "No active session folder found (run receiver first, or start them together)."
      echo "Falling back to stdout for stats."
      exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" "${@:2}"
    fi
    LOG_FILE="${SESSION_DIR}/streamer.log"
    exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" \
      --stats-log "${LOG_FILE}" "${@:2}"
    ;;

  receiver)
    COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
    BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    SESSION_DIR="${BASE_LOG_DIR}/${TIMESTAMP}_${COMMIT}_${BRANCH}"
    mkdir -p "$SESSION_DIR"

    # Update "latest" symlink
    ln -sfn "$SESSION_DIR" "${BASE_LOG_DIR}/latest"

    # Write session metadata
    {
      printf "date:   %s\n" "$(date)"
      printf "commit: %s\n" "$(git rev-parse HEAD 2>/dev/null || echo unknown)"
      printf "branch: %s\n" "$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
      printf "preset: %s\n" "$PRESET"
    } > "${SESSION_DIR}/info.log"

    echo "Session: ${SESSION_DIR}"
    exec "${BUILD_DIR}/streaming_receiver/Release/streaming_receiver" \
      --stats-log "${SESSION_DIR}/receiver.log" "${@:2}"
    ;;

  *)
    echo "Usage: $0 {server|streamer|receiver} [extra args]"
    exit 1
    ;;
esac
