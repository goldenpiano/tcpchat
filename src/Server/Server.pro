TEMPLATE = app
TARGET = Server

QT += network core gui widgets

INCLUDEPATH += .

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    server.h
FORMS += \
    server.ui
SOURCES += \
    main.cpp \
    server.cpp
