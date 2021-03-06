QT += testlib network sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

TARGET = networkTests

SOURCES += \
    testutils.cpp \
    tst_testsnakeserver.cpp

CONFIG(release, debug|release): {
    DESTDIR = $$PWD/build/release

} else {
    DESTDIR = $$PWD/build/debug
}

include($$PWD/../NetworkProtocol/NetworkProtocol.pri)


HEADERS += \
    testutils.h

deployTest.commands = cqtdeployer -bin $$DESTDIR clear -qmake $$QMAKE_QMAKE -targetDir $$PWD/deployTests -libDir $$PWD/../../ -recursiveDepth 5

test.depends = deployTest
test.commands = $$PWD/deployTests/networkTests.sh

QMAKE_EXTRA_TARGETS += \
    deployTest \
    test
