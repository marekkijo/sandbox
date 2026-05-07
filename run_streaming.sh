#!/usr/bin/env bash
# Streaming launcher.
# Usage:
#   ./run_streaming.sh server   [extra args]
#   ./run_streaming.sh streamer [extra args]
#   ./run_streaming.sh receiver [extra args]
#
# The receiver creates a timestamped session folder in benchmark_logs/ and
# writes its path to benchmark_logs/.current. The streamer discovers this
# file lazily (on first stats flush) and writes streamer.log there.
# Start the receiver before or alongside the streamer.

set -euo pipefail

PRESET=${STREAMING_PRESET:-ninja}
BUILD_DIR="build/${PRESET}/streaming"
BASE_LOG_DIR="$(pwd)/benchmark_logs"

case "${1:-}" in
  server)
    exec "${BUILD_DIR}/streaming_signaling_server/Release/streaming_signaling_server" "${@:2}"
    ;;

  streamer)
    exec "${BUILD_DIR}/streaming_streamer/Release/streaming_streamer" \
      --stats-log-dir "${BASE_LOG_DIR}" "${@:2}"
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
