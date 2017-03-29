#-------------------------------------------------
#
# Project created by QtCreator 2017-03-27T09:29:53
#
#-------------------------------------------------

QT       += core gui network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestApp
TEMPLATE = app
CONFIG += c++11
QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 0


SOURCES += main.cpp\
        mainwindow.cpp \
    scanip.cpp \
    onofftestwidget.cpp \
    protocol/analysisthread.cpp \
    protocol/customprotocol.cpp \
    protocol/reciverhandle.cpp \
    protocol/senderorder.cpp \
    protocol/timersendthread.cpp \
    protocol/Math.cpp \
    webview/Webmainwindow.cpp

HEADERS  += mainwindow.h \
    scanip.h \
    onofftestwidget.h \
    protocol/analysisthread.h \
    protocol/customprotocol.h \
    protocol/reciverhandle.h \
    protocol/senderorder.h \
    protocol/timersendthread.h \
    protocol/Math.h \
    webview/Webmainwindow.h

FORMS    += mainwindow.ui \
    onofftestwidget.ui

RESOURCES += \
    image.qrc \
    webview/jquery.qrc

DISTFILES += \
    webview/jquery.min.js
