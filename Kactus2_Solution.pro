# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
# message("You are running qmake on a generated .pro file. This may not work!")


TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += IPXACTmodels/IPXACTmodels.pro \
    Kactus2.pro \
    Plugins/MCAPICodeGenerator/MCAPICodeGenerator.pro \
    Plugins/CppSourceAnalyzer/CppSourceAnalyzer.pro \
    Plugins/VHDLAnalyzer/VHDLSourceAnalyzer.pro \
    Plugins/MemoryMapHeaderGenerator/MemoryMapHeaderGenerator.pro \
    Plugins/ModelSimGenerator/ModelSimGenerator.pro \
    Plugins/AlteraBSPGenerator/AlteraBSPGenerator.pro \
    Plugins/QuartusPinImportPlugin/QuartusPinImportPlugin.pro \
    Plugins/PadsPartGenerator/PadsPartGenerator.pro \
    Plugins/MakefileGenerator/MakefileGenerator.pro \
    Plugins/VerilogGenerator/VerilogGeneratorPlugin.pro \
    Plugins/VHDLimport/VHDLimport.pro \
    Plugins/VerilogImport/VerilogImport.pro \
    Plugins/VerilogIncludeImport/VerilogIncludeImport.pro \
    Plugins/VerilogSourceAnalyzer/VerilogSourceAnalyzer.pro \
    Plugins/QuartusProjectGenerator/QuartusProjectGenerator.pro \
	Plugins/MemoryViewGenerator/MemoryViewGenerator.pro

CONFIG(test) {
	SUBDIRS += tests/test_all.pro
}

INSTALLS += help doc upgrade config library icons desktop

