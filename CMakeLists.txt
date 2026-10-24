cmake_minimum_required(VERSION 3.16)
project(tqztr_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(WIN32)
    set(TQZTR_PLATFORM "windows")
elseif(APPLE)
    set(TQZTR_PLATFORM "macos")
else()
    set(TQZTR_PLATFORM "linux")
endif()
message(STATUS "TQZTR Platform: ${TQZTR_PLATFORM}")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
find_package(JNI REQUIRED)

# ===== QScintilla =====
find_package(qscintilla2 CONFIG QUIET)
if(qscintilla2_FOUND)
    set(QSCINTILLA_LIB qscintilla2::qscintilla2)
    set(QSCINTILLA_INCLUDE "")
    message(STATUS "QScintilla found via find_package (vcpkg/Homebrew)")
else()
    find_library(QSCINTILLA_LIB
        NAMES qscintilla2_qt6 qscintilla2-qt6
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/homebrew/lib
            /opt/homebrew/opt/qscintilla2/lib
    )
    find_path(QSCINTILLA_INCLUDE
        NAMES Qsci/qsciscintilla.h
        PATHS
            /usr/include/qt6
            /usr/local/include/qt6
            /opt/homebrew/include/qt6
            /opt/homebrew/opt/qscintilla2/include/qt6
    )
    if(NOT QSCINTILLA_LIB OR NOT QSCINTILLA_INCLUDE)
        message(FATAL_ERROR "QScintilla not found. Install: linux='qscintilla-qt6', macos='brew install qscintilla2', windows='vcpkg install qscintilla'")
    endif()
    message(STATUS "QScintilla found via manual find")
endif()

# ===== qtermwidget (tylko Linux) =====
if(TQZTR_PLATFORM STREQUAL "linux")
    pkg_check_modules(QTERMWIDGET IMPORTED_TARGET qtermwidget6)
    if(QTERMWIDGET_FOUND)
        set(TQZTR_HAS_QTERMWIDGET ON)
        message(STATUS "qtermwidget found — using native terminal")
    else()
        message(STATUS "qtermwidget NOT found — using QProcess fallback terminal")
    endif()
endif()

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

target_include_directories(tqztr_editor PRIVATE
    src
    ${JNI_INCLUDE_DIRS}
    ${QSCINTILLA_INCLUDE}
)

target_link_libraries(tqztr_editor PRIVATE
    Qt6::Widgets
    ${QSCINTILLA_LIB}
    ${JNI_LIBRARIES}
)

if(TQZTR_HAS_QTERMWIDGET)
    target_compile_definitions(tqztr_editor PRIVATE TQZTR_HAS_QTERMWIDGET=1)
    target_link_libraries(tqztr_editor PRIVATE PkgConfig::QTERMWIDGET)
endif()

if(TQZTR_PLATFORM STREQUAL "linux" AND DEFINED JAVA_HOME)
    set_target_properties(tqztr_editor PROPERTIES
        BUILD_RPATH "${JAVA_HOME}/lib/server"
        INSTALL_RPATH "${JAVA_HOME}/lib/server"
    )
elseif(TQZTR_PLATFORM STREQUAL "macos" AND DEFINED JAVA_HOME)
    set_target_properties(tqztr_editor PROPERTIES
        BUILD_RPATH "${JAVA_HOME}/lib/server"
        INSTALL_RPATH "@executable_path/../lib"
    )
endif()
