#
# Copyright (C) 2018 - 2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(SERVER_LIB):error("Server.pri already included")
SERVER_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    SERVER_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    SERVER_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

LIBS += -L$$SERVER_LIB_OUTPUT_DIR -lServer

INCLUDEPATH += "$$PWD/"



