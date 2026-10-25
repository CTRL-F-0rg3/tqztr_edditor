
# TQZTR Editor

A custom, lightweight, and highly customizable code editor built with C++ and Qt6. Designed for massive projects (870k+ lines of code) with heavy customization, a true OLED theme, and a Kotlin-based plugin system.

## Features

- **OLED Theme** - True pitch-black UI (`#000000`) with vibrant magenta accents. Easy on the eyes and battery-friendly on OLED displays.
- **Vertical Tabs** - Zen-browser style tab sidebar on the left. Better overview of open files, especially in large projects.
- **Frameless Window** - macOS-style custom title bar with traffic-light buttons (close, minimize, maximize) and working edge-resizing.
- **Built for Performance** - Powered by QScintilla. Handles files with 870,000+ lines of code without lag. Wrapping disabled, lazy loading, incremental lexers.
- **Syntax Highlighting** - Out-of-the-box support for 20+ languages: Rust, C, C++, Go, Zig, D, Odin, Nim, Java, C#, Kotlin, TypeScript, JavaScript, Python, Ruby, JSON, Markdown, HTML, XML, CSS, SQL, YAML, TOML, INI, Bash, Makefile, CMake, GLSL, Dockerfile.
- **Custom Background** - Set any image (PNG, JPG, WebP, BMP) as a subtle backdrop behind your code with adjustable opacity.
- **Session Persistence** - Remembers open tabs, cursor positions, window geometry, splitter sizes, and background image. Everything comes back on restart.
- **Terminal Dock** - Multi-tab built-in terminal. Uses `qtermwidget` on Linux for full PTY support. Falls back to `QProcess` on other platforms.
- **Plugin System** - Extend the editor with Kotlin or Java plugins via JNI. Plugins can register menu actions, read the current buffer, insert text, and draw diagnostic markers.
- **C/C++ Analyzer Plugin** - Included Kotlin plugin that runs `clang -fsyntax-only` and displays error/warning markers in the editor margin.
- **File Manager** - Built-in tree view with context menu (New File, New Folder, Rename, Delete, Refresh).

## Screenshots

> Add your screenshots here. Recommended size: 1280x720 or larger.

```
[screenshot.png]
```

## Requirements

### Linux (Arch)

```bash
sudo pacman -S qt6-base qscintilla-qt6 qtermwidget6 jre-openjdk cmake
```

### Linux (Debian/Ubuntu)

```bash
sudo apt install qt6-base-dev libqscintilla2-qt6-dev libqtermwidget6-2-dev default-jdk cmake build-essential
```

### Optional

- `clang` - for the C/C++ analyzer plugin
- `kotlinc` - for building Kotlin plugins
- `JetBrains Mono`, `Fira Code` or `DejaVu Sans Mono` - recommended fonts

## Build

```bash
git clone https://github.com/CTRL-F-0rg3/tqztr_edditor.git
cd tqztr_edditor
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

The binary will be at `build/tqztr_editor`.

Or use the helper script:

```bash
./scripts/build.sh
```

## Run

```bash
./build/tqztr_editor
```

## Packaging

To create a distributable archive:

```bash
./scripts/package.sh v0.1.0
```

Result: `dist/tqztr-editor-linux-v0.1.0.tar.gz`

The archive contains:
- `bin/tqztr_editor` - compiled executable
- `src/` - full C++ source and CMakeLists.txt
- `plugins/` - any compiled Kotlin plugins

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+O` | Open file |
| `Ctrl+Shift+O` | Open folder |
| `Ctrl+S` | Save file |
| `Ctrl+T` | New terminal |
| `Ctrl+`` | Toggle terminal dock |
| `Ctrl+Q` | Quit |

## Plugin System

Plugins are written in Kotlin or Java, compiled to `.jar`, and dropped into `~/.config/tqztr_editor/plugins/`. The editor loads them via JNI on startup.

### Example Plugin

```kotlin
object HelloPlugin {
    @JvmStatic
    fun onLoad() {
        TqztrNative.registerAction("Tools", "Say Hello", "HelloPlugin")
    }

    @JvmStatic
    fun onInvoke() {
        TqztrNative.showMessage("Hello from Kotlin!")
    }
}
```

### Available Native API

| Method | Description |
|--------|-------------|
| `showMessage(msg: String)` | Show a message box |
| `currentFilePath(): String` | Path of the current file |
| `insertText(text: String)` | Insert text at the cursor |
| `registerAction(menu, label, handlerClass)` | Register a menu action |
| `addDiagnostic(line, col, severity, msg)` | Add a diagnostic marker (0=note, 1=warning, 2=error) |
| `clearDiagnostics()` | Clear all diagnostic markers |

### Build a Plugin

```bash
kotlinc plugins-src/CPlugin/TqztrNative.kt \
        plugins-src/CPlugin/CPlugin.kt \
        -include-runtime \
        -d ~/.config/tqztr_editor/plugins/CPlugin.jar
```

Or use the helper:

```bash
./scripts/build_plugin.sh
```

## Configuration

- Session file: `~/.config/tqztr_editor/session.json`
- Plugins directory: `~/.config/tqztr_editor/plugins/`

Delete `session.json` to reset window state.

## Project Structure

```
tqztr_editor/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── src/
│   ├── main.cpp                 # Entry point, Qt theme
│   ├── main_window.{hpp,cpp}    # Main window, menus, file tree, sessions
│   ├── editor.{hpp,cpp}         # QScintilla wrapper, OLED theme, background
│   ├── title_bar.{hpp,cpp}      # macOS-style title bar
│   ├── terminal.{hpp,cpp}       # Terminal dock widget
│   ├── languages.{hpp,cpp}      # Syntax highlighting (20+ languages)
│   ├── session.{hpp,cpp}        # Session persistence (JSON)
│   └── plugin_manager.{hpp,cpp} # JNI plugin loader
├── plugins-src/
│   └── CPlugin/
│       ├── TqztrNative.kt       # Native API declaration
│       └── CPlugin.kt           # Clang-based analyzer
└── scripts/
    ├── build.sh
    ├── package.sh
    └── build_plugin.sh
```

## Known Issues

- Plugin system requires `JAVA_HOME` to be set correctly and `libjvm.so` in the library path.
- Terminal may have minor layout quirks on Wayland due to Qt6 `qtermwidget` behavior.
- Windows and macOS builds are experimental. Native terminal falls back to `QProcess` (no PTY, limited interactivity).
- `YAML`, `TOML`, `Dockerfile`, `Nix` use basic fallback lexers - comments starting with `#` may not be highlighted in all cases.

## Roadmap

- [ ] LSP client (clangd, rust-analyzer)
- [ ] Command palette (`Ctrl+Shift+P`)
- [ ] Project-wide search (ripgrep integration)
- [ ] Git integration (status, diff, commit)
- [ ] File icons in tree view
- [ ] Windows/macOS native terminal (ConPTY, PTY)
## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.

You are free to use, modify, and distribute this software under the terms of the GPLv3. Any derivative work must also be open-source and distributed under the same license.

See the [LICENSE](LICENSE) file for the full legal text.
## Contributing

Pull requests welcome. For major changes, open an issue first to discuss what you would like to change.

## Acknowledgments

- [QScintilla](https://riverbankcomputing.com/software/qscintilla) - text editor component
- [QTermWidget](https://github.com/lxqt/qtermwidget) - terminal widget
- [Qt6](https://www.qt.io/) - cross-platform framework
- [Catppuccin](https://github.com/catppuccin) - color palette inspiration

