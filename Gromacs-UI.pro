QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/command/createbox.cpp \
    src/command/creategromacsmodel.cpp \
    src/command/executor.cpp \
    src/command/neutralise.cpp \
    src/command/queue.cpp \
    src/command/runsimulation.cpp \
    src/command/solvate.cpp \
    src/fileserver.cpp \
    src/gromacsconfigfilegenerator.cpp \
    src/logforwarder.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/model/systemsetup.cpp \
    src/pdbconverter.cpp \
    src/pdbdownloader.cpp \
    src/pdbinfoextractor.cpp \
    src/preferencesdialog.cpp \
    src/model/project.cpp \
    src/projectmanager.cpp \
    src/settings.cpp \
    src/simulationsetupform.cpp \
    src/statusmessagesetter.cpp \
    src/model/simulation.cpp \
    src/systemsetupform.cpp \
    src/uiconnectionhelper.cpp

HEADERS += \
    src/command/createbox.h \
    src/command/creategromacsmodel.h \
    src/command/executor.h \
    src/command/neutralise.h \
    src/command/queue.h \
    src/command/runsimulation.h \
    src/command/solvate.h \
    src/fileserver.h \
    src/gromacsconfigfilegenerator.h \
    src/logforwarder.h \
    src/mainwindow.h \
    src/pdbconverter.h \
    src/pdbdownloader.h \
    src/pdbinfoextractor.h \
    src/preferencesdialog.h \
    src/model/project.h \
    src/projectmanager.h \
    src/settings.h \
    src/simulationsetupform.h \
    src/statusmessagesetter.h \
    src/model/simulation.h \
    src/model/systemsetup.h \
    src/systemsetupform.h \
    src/uiconnectionhelper.h

FORMS += \
    ui/simulationsetupform.ui \
    ui/systemsetupform.ui \
    ui/mainwindow.ui \
    ui/preferencesdialog.ui

TRANSLATIONS += \
    share/Gromacs-UI_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

DISTFILES += \
    share/gromacs.png
