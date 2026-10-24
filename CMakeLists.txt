cmake_minimum_required(VERSION 3.16)
project(tqztr_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
find_package(JNI REQUIRED)                 # <-- szuka jni.h i libjvm.so

pkg_check_modules(QTERMWIDGET REQUIRED IMPORTED_TARGET qtermwidget6)

add_executable(tqztr_editor
    src/main.cpp
    src/main_window.cpp
    src/editor.cpp
    src/title_bar.cpp
    src/terminal.cpp
    src/languages.cpp
    src/session.cpp
    src/plugin_manager.cpp                  # <-- nowy
)
target_include_directories(tqztr_editor PRIVATE src ${JNI_INCLUDE_DIRS})
target_link_libraries(tqztr_editor PRIVATE
    Qt6::Widgets
    qscintilla2_qt6
    PkgConfig::QTERMWIDGET
    ${JNI_LIBRARIES}
)

# Rpath do libjvm.so — bez tego linker znajdzie, ale runtime nie
set_target_properties(tqztr_editor PROPERTIES
    BUILD_RPATH "${JAVA_HOME}/lib/server"
    INSTALL_RPATH "${JAVA_HOME}/lib/server"
)