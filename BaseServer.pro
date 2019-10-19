
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    QuasarAppLib \
    ClientProtocol \
    ProtocolTests \

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
