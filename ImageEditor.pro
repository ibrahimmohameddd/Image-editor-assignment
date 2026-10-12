QT += widgets svg svgwidgets

CONFIG += c++17
TARGET = ImageEditor
TEMPLATE = app

# Windows: no console window
CONFIG += windows

SOURCES += \
    main_qt.cpp \
    mainwindow.cpp \
    filters.cpp

HEADERS += \
    mainwindow.h \
    filters.h \
    icons.h \
    Image_Class.h \
    stb_image.h \
    stb_image_write.h

# Build output
DESTDIR = build_pro

# Include path
INCLUDEPATH += $$PWD

# Windows deploy
win32 {
    # Suppress console window
    CONFIG += windows
}

# C++ flags
QMAKE_CXXFLAGS += -std=c++17
