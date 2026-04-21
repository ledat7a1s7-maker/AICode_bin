# SmartTrashCanQt.pro - Qt Project Configuration File
# Smart Trash Can Control System with Qt 6

QT       += core gui network
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Application name
TARGET = SmartTrashCanQt
TEMPLATE = app

# Source files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    camerastream.cpp \
    aidetection.cpp \
    robotcontroller.cpp \
    mecanumcontrol.cpp

# Header files
HEADERS += \
    mainwindow.h \
    camerastream.h \
    aidetection.h \
    robotcontroller.h \
    mecanumcontrol.h \
    config.h

# UI files
FORMS += \
    mainwindow.ui

# Resources
RESOURCES += \
    resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Windows specific
win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    RC_FILE =
}

# Linux specific
unix:!macx {
    DEFINES += LINUX
}

# Disable deprecated warnings for Qt 6
greaterThan(QT_MAJOR_VERSION, 5) {
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
}
