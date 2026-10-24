#!/usr/bin/env bash
set -e

echo "=== TQZTR Editor: JVM Plugin Environment Setup ==="

# 1. Sprawdź/ zainstaluj JDK
if ! command -v java &> /dev/null; then
    echo "[1/3] Java not found. Installing via SDKMAN..."
    if [ ! -d "$HOME/.sdkman" ]; then
        curl -s "https://get.sdkman.io" | bash
    fi
    source "$HOME/.sdkman/bin/sdkman-init.sh"
    sdk install java 21.0.2-tem
else
    echo "[1/3] Java found: $(java -version 2>&1 | head -n 1)"
fi

# 2. Sprawdź/ zainstaluj Kotlin (jeśli brak)
if ! command -v kotlinc &> /dev/null; then
    echo "[2/3] Kotlin not found. Installing via SDKMAN..."
    source "$HOME/.sdkman/bin/sdkman-init.sh"
    sdk install kotlin
else
    echo "[2/3] Kotlin found: $(kotlinc -version 2>&1)"
fi

# 3. Zbuduj wtyczki (każdy podkatalog z build.gradle lub *.kt)
echo "[3/3] Building plugins..."
PLUGIN_SRC="$(dirname "$0")/../plugins-src"
OUT_DIR="$HOME/.config/tqztr_editor/plugins"
mkdir -p "$OUT_DIR"

if [ -d "$PLUGIN_SRC" ]; then
    for plugin in "$PLUGIN_SRC"/*/; do
        if [ -f "${plugin}build.gradle.kts" ]; then
            echo "  -> Gradle build: $plugin"
            (cd "$plugin" && gradle build --quiet)
            # Skopiuj wynikowy JAR
            find "${plugin}build/libs" -name "*.jar" -exec cp {} "$OUT_DIR/" \;
        elif [ -f "${plugin}"*.kt ]; then
            echo "  -> Kotlinc build: $plugin"
            kotlinc "${plugin}"*.kt -include-runtime -d "${OUT_DIR}/$(basename "$plugin").jar"
        fi
    done
else
    echo "  (Brak katalogu plugins-src — pomijam budowanie)"
fi

echo "=== Done. Plugins available in $OUT_DIR ==="