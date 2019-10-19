
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    QuasarAppLib \
    ClientProtocol \
    Tests \

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
