
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    QuasarAppLib \
    ServerProtocol \
    ClientProtocol \
    Terminal \
    Server \
    Daemon \
    serverProtocolTests \

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
