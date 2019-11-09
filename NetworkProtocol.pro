
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    QuasarAppLib \
    NetworkProtocol \
    ProtocolTests \

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
