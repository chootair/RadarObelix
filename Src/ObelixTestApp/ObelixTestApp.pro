# ----------------------------------------------------------
# INCLUDE
include(../../RadarObelix.pri)

# ----------------------------------------------------------
# CONFIG
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

# ----------------------------------------------------------
# TARGET
DESTDIR = $${ROOT_FOLDER}/Bin
TARGET = ObelixTestApp$${SUFFIX}
TEMPLATE = app

# ----------------------------------------------------------
# SOURCES
SOURCES += main.cpp \
           ColorPickerButton.cpp \
           MainWindow.cpp

# ----------------------------------------------------------
# HEADERS
HEADERS += MainWindow.h \
  ColorPickerButton.h

# ----------------------------------------------------------
# FORMS
FORMS += MainWindow.ui

# ----------------------------------------------------------
# LIBS
INCLUDEPATH += ../ObelixLib
LIBS += -L$${ROOT_FOLDER}/Lib -lObelixLib
