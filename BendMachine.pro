#-------------------------------------------------
#
# Project created by QtCreator 2017-10-23T14:29:52
#
#-------------------------------------------------

QT       += core gui opengl printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BendMachine
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bendglwidget.cpp \
    globalVar.cpp \
    createworkpiece.cpp

HEADERS  += mainwindow.h \
    SysGLData.h \
    bendglwidget.h \
    globalVar.h \
    createworkpiece.h

FORMS    += mainwindow.ui
