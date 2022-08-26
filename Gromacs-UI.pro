QT       += core gui webenginewidgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += object_parallel_to_source


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/appprovider.cpp \
    src/filecontentviewer.cpp \
    src/gromacsconfigfilegenerator.cpp \
    src/logforwarder.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/pdbdownloader.cpp \
    src/pdbinfoextractor.cpp \
    src/projectmanager.cpp \
    src/simulationstatuschecker.cpp \
    src/settings.cpp \
    src/statusmessagesetter.cpp \
    src/form/connectionhelper.cpp \
    src/form/preferencesdialog.cpp \
    src/form/progresschart.cpp \
    src/form/simulationsetupform.cpp \
    src/form/systemsetupform.cpp \
    src/form/temperaturegroupconfigform.cpp \
    src/model/project.cpp \
    src/model/serializable.cpp \
    src/model/simulation.cpp \
    src/model/systemsetup.cpp \
    src/model/temperaturecouplinggroup.cpp \
    $$files(src/command/*.cpp) \
    $$files(src/config/*.cpp) \
    $$files(src/pipeline/*.cpp, true)

HEADERS += \
    src/appprovider.h \
    src/filecontentviewer.h \
    src/gromacsconfigfilegenerator.h \
    src/logforwarder.h \
    src/mainwindow.h \
    src/pdbdownloader.h \
    src/pdbinfoextractor.h \
    src/projectmanager.h \
    src/settings.h \
    src/simulationstatuschecker.h \
    src/statusmessagesetter.h \
    src/form/connectionhelper.h \
    src/form/preferencesdialog.h \
    src/form/progresschart.h \
    src/form/simulationsetupform.h \
    src/form/systemsetupform.h \
    src/form/temperaturegroupconfigform.h \
    src/misc/bimaphelper.h \
    src/model/project.h \
    src/model/serializable.h \
    src/model/simulation.h \
    src/model/systemsetup.h \
    src/model/temperaturecouplinggroup.h \
    $$files(src/command/*.h) \
    $$files(src/config/*.h) \
    $$files(src/pipeline/*.h, true)


FORMS += \
    ui/temperaturegroupconfigform.ui \
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

isEmpty( PREFIX ) {
  PREFIX = /usr/local
}

TARGET=gromacs-ui
target.path = $${PREFIX}/bin
INSTALLS += target

molviewer.path = $${PREFIX}/share/gromacs-ui
molviewerhtml = viewer.html
molviewer.files += share/$${molviewerhtml}
molviewer.files += molstar/build/viewer/molstar.js
molviewer.files += molstar/build/viewer/molstar.css
DEFINES += MOLVIEWER=\\\"$${molviewer.path}/$${molviewerhtml}\\\"
INSTALLS += molviewer
