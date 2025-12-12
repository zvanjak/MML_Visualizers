#!/bin/bash

# Wrapper script to run Qt visualizers on Linux with clean environment to avoid snap conflicts

# Set MML_PROJECT_PATH if not already set
if [ -z "$MML_PROJECT_PATH" ]; then
    export MML_PROJECT_PATH="$(cd "$(dirname "$0")/.." && pwd)"
fi

# Run with minimal environment to avoid snap library conflicts
env -i \
    HOME="$HOME" \
    DISPLAY="${DISPLAY:-:0}" \
    XAUTHORITY="${XAUTHORITY:-$HOME/.Xauthority}" \
    PATH="/usr/local/bin:/usr/bin:/bin" \
    LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu:/usr/lib" \
    QT_QPA_PLATFORM_PLUGIN_PATH="/usr/lib/x86_64-linux-gnu/qt6/plugins" \
    MML_PROJECT_PATH="$MML_PROJECT_PATH" \
    "$@"
