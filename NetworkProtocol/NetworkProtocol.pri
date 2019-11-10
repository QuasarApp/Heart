#
# Copyright (C) 2018 - 2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(NETWORKPROTOCOL_LIB):error("NetworkProtocol.pri already included")
NETWORKPROTOCOL_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    NETWORKPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    NETWORKPROTOCOL_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

LIBS += -L$$NETWORKPROTOCOL_LIB_OUTPUT_DIR -lNetworkProtocol

include($$PWD/NetworkProtocolIncludes.pri)



