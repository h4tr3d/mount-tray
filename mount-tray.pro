#-------------------------------------------------
#
# Project created by QtCreator 2010-11-10T01:37:08
#
#-------------------------------------------------

QT        += core gui

TARGET     = mount-tray
TEMPLATE   = app

CONFIG    += link_pkgconfig qdbus
PKGCONFIG += libudev

SOURCES += \
    src/mounttrayapp.cpp \
    src/main.cpp \
    src/storagemanager.cpp \
    src/storageitem.cpp \
    src/mount.cpp \
    src/diskmonitor.cpp \
    src/menudiskitem.cpp

HEADERS  += \
    src/mounttrayapp.h \
    src/storagemanager.h \
    src/storageitem.h \
    src/mount.h \
    src/diskmonitor.h \
    src/menudiskitem.h

RESOURCES += \
    main.qrc

FORMS += \
    src/menudiskitem.ui
