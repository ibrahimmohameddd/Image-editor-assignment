cmake_minimum_required(VERSION 3.20)
project(ImageEditor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Auto-detect Qt6 in common Windows install locations ─────────────────────
# Add any extra paths below if Qt6 is installed elsewhere.
set(QT6_SEARCH_PATHS
    "C:/Qt/6.9.0/mingw_64"
    "C:/Qt/6.8.0/mingw_64"
    "C:/Qt/6.7.0/mingw_64"
    "C:/Qt/6.6.0/mingw_64"
    "C:/Qt/6.5.0/mingw_64"
    "I:/Qt/6.9.0/mingw_64"
    "I:/Qt/6.8.0/mingw_64"
    "I:/Qt/6.7.0/mingw_64"
    "I:/Qt6/6.9.0/mingw_64"
    "I:/Qt6/6.8.0/mingw_64"
    "$ENV{USERPROFILE}/Qt/6.9.0/mingw_64"
    "$ENV{USERPROFILE}/Qt/6.8.0/mingw_64"
    "$ENV{USERPROFILE}/Qt/6.7.0/mingw_64"
    "$ENV{LOCALAPPDATA}/Qt/6.9.0/mingw_64"
    "$ENV{LOCALAPPDATA}/Qt/6.8.0/mingw_64"
)

# If CMAKE_PREFIX_PATH is not already set externally, try to find Qt6
if(NOT CMAKE_PREFIX_PATH)
    foreach(QT_PATH ${QT6_SEARCH_PATHS})
        if(EXISTS "${QT_PATH}/lib/cmake/Qt6")
            set(CMAKE_PREFIX_PATH "${QT_PATH}" CACHE PATH "Qt6 prefix path" FORCE)
            message(STATUS "Auto-detected Qt6 at: ${QT_PATH}")
            break()
        endif()
    endforeach()
endif()

# ─── Qt6 setup ────────────────────────────────────────────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg SvgWidgets)

# ─── Source files ─────────────────────────────────────────────────────────────
set(SOURCES
    main.cpp
    mainwindow.cpp
    filters.cpp
)

set(HEADERS
    mainwindow.h
    filters.h
    icons.h
    Image_Class.h
)

# ─── Executable ───────────────────────────────────────────────────────────────
if(WIN32)
    add_executable(ImageEditor WIN32 ${SOURCES} ${HEADERS})
else()
    add_executable(ImageEditor ${SOURCES} ${HEADERS})
endif()

target_include_directories(ImageEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ImageEditor PRIVATE
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
)

# ─── Windows: deploy Qt DLLs alongside the exe ───────────────────────────────
if(WIN32)
    # Find windeployqt tool
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

    add_custom_command(TARGET ImageEditor POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:ImageEditor>"
        COMMENT "Deploying Qt runtime DLLs..."
        VERBATIM
    )
endif()

# ─── Copy images/ directory to build directory ────────────────────────────────
add_custom_command(TARGET ImageEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:ImageEditor>/images"
    COMMENT "Creating images/ directory in build output"
)
