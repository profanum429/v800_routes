#-------------------------------------------------
#
# Project created by QtCreator 2014-11-01T10:09:06
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = v800_routes
TEMPLATE = app

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

CONFIG(debug,debug|release) DESTDIR = debug
CONFIG(release,debug|release) DESTDIR = release
MOC_DIR = $$DESTDIR/tmp
OBJECTS_DIR = $$DESTDIR/tmp
RCC_DIR = $$DESTDIR/tmp
UI_DIR = $$DESTDIR/tmp

include(src/src.pri)
