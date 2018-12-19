#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T22:25:56
#
#-------------------------------------------------

QT       += core gui network sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET              = ComTool
TEMPLATE            = app
MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin
win32:RC_FILE       = main.rc
PRECOMPILED_HEADER  = myhelper.h

include($$PWD/qextserialport/qextserialport.pri)

SOURCES += main.cpp\
        frmcomtool.cpp \
        app.cpp \
    picturebox.cpp

HEADERS  +=\
        frmcomtool.h \
        app.h \
    myhelper.h \
    picturebox.h

FORMS    += frmcomtool.ui

RESOURCES += \
    main.qrc

CONFIG += qt warn_off release
