
QT += core gui xml testlib qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-rmc
TEMPLATE = app

INCLUDEPATH += $$PWD/../qt-plus/source/cpp

SOURCES += \
    main.cpp \
    CApplication.cpp

HEADERS += \
    CApplication.h

# Directories
DESTDIR = $$PWD/../bin
OBJECTS_DIR = $$PWD/obj
MOC_DIR = $$PWD/moc
RCC_DIR = $$PWD/rcc
UI_DIR = $$PWD/ui

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Copy qt-plus to bin
win32 {
    copyfile = $$PWD/../qt-plus/bin/*.dll
    copydest = $$PWD/../bin
} else {
    copyfile = $$PWD/../qt-plus/bin/*.so
    copydest = $$PWD/../bin
}

QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)

# Copy * to bin2
win32 {
    copyfile = $$PWD/../bin/*
    copydest = $$PWD/../bin2
} else {
    copyfile = $$PWD/../bin/*
    copydest = $$PWD/../bin2
}

QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)
