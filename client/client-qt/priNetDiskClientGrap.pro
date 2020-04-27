#-------------------------------------------------
#
# Project created by QtCreator 2018-03-09T19:42:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = priNetDiskClientGrap
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    configdialog.cpp \
    configfile.cpp \
    prinetdiskclientter.cpp
HEADERS += \
        mainwindow.h \
    configdialog.h \
    configfile.h \
    prinetdiskclientter.h

FORMS += \
        mainwindow.ui \
    configdialog.ui

RESOURCES += \
    res.qrc

LIBS += -ludt

unix:!macx: LIBS += -L$$PWD/../../lib/libnwm-udt-client/ -lnwm-udt-client

INCLUDEPATH += $$PWD/../../lib/libnwm-udt-client/
DEPENDPATH += $$PWD/../../lib/libnwm-udt-client/

unix:!macx: LIBS += -L$$PWD/../../lib/libencrypt -lencrypt

INCLUDEPATH += $$PWD/../../lib/libencrypt
DEPENDPATH += $$PWD/../../lib/libencrypt
