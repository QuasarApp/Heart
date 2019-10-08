#
# Copyright (C) 2018 - 2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(SERVERPROTOCOL_LIB):error("ServerProtocol.pri already included")
SERVERPROTOCOL_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    SERVERPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    SERVERPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

LIBS += -L$$SERVERPROTOCOL_LIB_OUTPUT_DIR -lServerProtocol

INCLUDEPATH += "$$PWD/"



