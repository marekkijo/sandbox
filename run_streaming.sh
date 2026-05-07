#!/usr/bin/env bash
# Streaming launcher.
# Usage:
#   ./run_streaming.sh server   [extra args]
#   ./run_streaming.sh streamer [extra args]
#   ./run_streaming.sh receiver [extra args]
#
# The receiver creates a timestamped session folder in benchmark_logs/ and
# records its path in benchmark_logs/.current. The streamer reads .current
# and receives the same path as --stats-log. Start the receiver first.

set -euo pipefail

PRESET=${STREAMING_PRESET:-ninja}
BUILD_DIR="build/${PRESET}/streaming"
BASE_LOG_DIR="$(pwd)/benchmark_logs"

case "${1:-}" in
  server)
    exec "${BUILD_DIR}/streaming_signaling_server/Release/streaming_signaling_server" "${@:2}"
    ;;

  streamer)
    SESSION_DIR=$(cat "${BASE_LOG_DIR}/.current" 2>/dev/null | tr -d '[:space:]')
    if [[ -z "$SESSION_DIR" ]]; then
      echo "No active session found. Start the receiver first."
      exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" "${@:2}"
    fi
    exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" \
      --stats-log "${SESSION_DIR}/streamer.log" "${@:2}"
    ;;

  receiver)
    BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null | tr '/\\' '__')
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    SESSION_DIR="${BASE_LOG_DIR}/${TIMESTAMP}_${BRANCH}"
    mkdir -p "$SESSION_DIR"

    # Record current session path for streamer to discover
    printf "%s\n" "$SESSION_DIR" > "${BASE_LOG_DIR}/.current"

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
