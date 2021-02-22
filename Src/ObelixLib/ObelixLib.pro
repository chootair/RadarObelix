# ----------------------------------------------------------
# INCLUDE
include(../../RadarObelix.pri)

# ----------------------------------------------------------
# CONFIG
QT      += widgets network
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += OBELIXLIB_LIBRARY


# ----------------------------------------------------------
# TARGET
DESTDIR = $${ROOT_FOLDER}/Lib
DLLDESTDIR = $${ROOT_FOLDER}/Bin
TARGET = ObelixLib$${SUFFIX}
TEMPLATE = lib

# ----------------------------------------------------------
# SOURCES
SOURCES +=  \
  ObelixClient1090Thread.cpp \
  PresistImage.cpp \
  ObelixPlotWidget.cpp \
  ObelixUdpPlotWidget.cpp \
  ObelixUdpReaderThread.cpp \
  ObelixUdpSim.cpp \
  ObelixUdpSimThread.cpp \
  ObelixToolbox.cpp

# ----------------------------------------------------------
# HEADERS
HEADERS +=  \
  ObelixClient1090Thread.h \
  PresistImage.h \
  Obelix.h \
  ObelixPlotWidget.h \
  ObelixUdpPlotWidget.h \
  ObelixUdpReaderThread.h \
  ObelixUdpSim.h \
  ObelixUdpSimThread.h \
  ObelixLibDef.h \
  ObelixToolbox.h
