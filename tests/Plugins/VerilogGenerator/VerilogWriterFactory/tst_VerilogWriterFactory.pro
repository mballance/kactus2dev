#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Janne Virtanen
# Date: 26.01.2017
#
# Description:
# Qt project file for running unit tests for VerilogWriterFactory.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VerilogWriterFactory

QT += core xml gui testlib
CONFIG += testcase console

DEFINES += VERILOGGENERATORPLUGIN_LIB

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$PWD/../../../../executable
INCLUDEPATH += $$PWD/../../../../executable/Plugins
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../executable
DEPENDPATH += $$PWD/../../../../executable/Plugins
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_VerilogWriterFactory.pri)
