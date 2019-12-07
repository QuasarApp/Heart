#
# Copyright (C) 2018 - 2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    QuasarAppLib \
    NetworkProtocol \
    ProtocolTests \

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
