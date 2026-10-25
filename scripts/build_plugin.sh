#!/usr/bin/env bash
set -e

PLUGIN_DIR="$HOME/.config/tqztr_editor/plugins"
mkdir -p "$PLUGIN_DIR"

echo "Kompilacja CPlugin.jar..."
kotlinc plugins-src/CPlugin/TqztrNative.kt plugins-src/CPlugin/CPlugin.kt -include-runtime -d "$PLUGIN_DIR/CPlugin.jar"
echo "Gotowe! Wtyczka w: $PLUGIN_DIR/CPlugin.jar"
