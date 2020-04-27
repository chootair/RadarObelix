# ----------------------------------------------------------
# INCLUDE
include(../../RadarObelix.pri)

# ----------------------------------------------------------
# CONFIG
QT      += widgets network
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG  += staticlib


# ----------------------------------------------------------
# TARGET
DESTDIR = $${ROOT_FOLDER}/Lib
TARGET = ObelixLib$${SUFFIX}
TEMPLATE = lib

# ----------------------------------------------------------
# SOURCES
SOURCES +=  \
  PresistImage.cpp \
  ObelixPlotWidget.cpp \
  ObelixUdpPlotWidget.cpp \
  ObelixUdpReader.cpp \
  ObelixUdpSim.cpp \
  ObelixUdpSimThread.cpp

# ----------------------------------------------------------
# HEADERS
HEADERS +=  \
  PresistImage.h \
  Obelix.h \
  ObelixPlotWidget.h \
  ObelixUdpPlotWidget.h \
  ObelixUdpReader.h \
  ObelixUdpSim.h \
  ObelixUdpSimThread.h
