INCLUDEPATH += "$$PWD/"
INCLUDEPATH += "$$PWD/DataPacakages"
INCLUDEPATH += "$$PWD/Tables"

LIBS += -L$$NETWORKPROTOCOL_LIB_OUTPUT_DIR -lcrypto

include($$PWD/../QuasarAppLib/QuasarLib.pri)