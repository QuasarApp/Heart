#-------------------------------------------------
#
# Project created by QtCreator 2019-02-16T12:24:27
#
#-------------------------------------------------

QT       -= gui
QT       += network sql concurrent

TARGET = Server
TEMPLATE = lib

DEFINES += SERVER_LIBRARY

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
    item.cpp \
    keysreactor.cpp \
    mainserver.cpp \
    playerdbdata.cpp \
    sqldbcache.cpp \
    sqldbwriter.cpp \
    websocketcontroller.cpp

TARGET = Server

CONFIG(release, debug|release): {
    DESTDIR = $$PWD/build/release

} else {
    DESTDIR = $$PWD/build/debug
}

HEADERS += \
        item.h \
        keysreactor.h \
        mainserver.h \
        playerdbdata.h \
        server_global.h \
        sqldbcache.h \
        sqldbwriter.h \
        websocketcontroller.h

include($$PWD/../../QuasarAppLib/QuasarLib.pri)
include($$PWD/../ServerProtocol/ServerProtocol.pri)
include($$PWD/../ClientProtocol/ClientProtocol.pri)
include($$PWD/../../SnakeUtils/SnakeUtils.pri)

RESOURCES += sqlres.qrc
