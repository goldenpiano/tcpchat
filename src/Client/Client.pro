TEMPLATE = app
TARGET = Client

QT += network core gui widgets

INCLUDEPATH += .

HEADERS += \
    client.h \
    packet.h
FORMS += \
    client.ui
SOURCES += main.cpp \
    client.cpp
