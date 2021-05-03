
!defined(ROOT_FOLDER,var) {
    ROOT_FOLDER=$${PWD}
}


# ----------------------------------------------------------
# DEBUG AND RELEASE
debug_and_release {
  CONFIG -= debug_and_release
  CONFIG += debug_and_release
}
  CONFIG(debug, debug|release) {
  CONFIG -= debug release
  CONFIG += debug
}
  CONFIG(release, debug|release) {
  CONFIG -= debug release
  CONFIG += release
}

debug {
    SUFFIX=_d
} else {
    SUFFIX=
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# ----------------------------------------------------------
# USE SHAPE LIBRARY
CONFIG += shapelib

shapelib {
message(Use Shapelib)
DEFINES += USESHAPELIB
SHAPELIB_DIR="C:\Data\Bibliotheques\shapelib-master"
SHAPELIB_BINDIR="C:\Data\Bibliotheques\build-shapelib-master-Desktop_Qt_5_12_3_MSVC2017_64bit-Release\dll"
}
