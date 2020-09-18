
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


