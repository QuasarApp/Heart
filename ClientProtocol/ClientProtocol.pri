#
# Copyright (C) 2018 - 2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(CLIENTPROTOCOL_LIB):error("ClientProtocol.pri already included")
CLIENTPROTOCOL_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    CLIENTPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    CLIENTPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

LIBS += -L$$CLIENTPROTOCOL_LIB_OUTPUT_DIR -lClientProtocol

include($$PWD/ClientProtocolIncludes.pri)



