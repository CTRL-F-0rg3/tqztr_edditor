cmake_minimum_required(VERSION 3.16)
project(tqztr_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
find_package(JNI REQUIRED)

# QScintilla - szukamy standardowo w Arch/Debian
find_library(QSCINTILLA_LIB NAMES qscintilla2_qt6 qscintilla2-qt6)
find_path(QSCINTILLA_INCLUDE NAMES Qsci/qsciscintilla.h PATHS /usr/include/qt6 /usr/include/x86_64-linux-gnu/qt6)

if(NOT QSCINTILLA_LIB OR NOT QSCINTILLA_INCLUDE)
    message(FATAL_ERROR "Brak QScintilla. Zainstaluj: sudo pacman -S qscintilla-qt6")
endif()

# qtermwidget
pkg_check_modules(QTERMWIDGET REQUIRED IMPORTED_TARGET qtermwidget6)

add_executable(tqztr_editor
    src/main.cpp
    src/main_window.cpp
    src/editor.cpp
    src/title_bar.cpp
    src/terminal.cpp
    src/languages.cpp
    src/session.cpp
    src/plugin_manager.cpp
)

target_include_directories(tqztr_editor PRIVATE src ${JNI_INCLUDE_DIRS} ${QSCINTILLA_INCLUDE})
target_link_libraries(tqztr_editor PRIVATE Qt6::Widgets ${QSCINTILLA_LIB} ${JNI_LIBRARIES} PkgConfig::QTERMWIDGET)
target_compile_definitions(tqztr_editor PRIVATE TQZTR_HAS_QTERMWIDGET=1)
