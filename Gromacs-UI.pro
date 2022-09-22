include(fix_moc.prf)

QT       += core gui webenginewidgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += object_parallel_to_source


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$files(src/*.cpp) \
    $$files(src/model/*.cpp) \
    $$files(src/command/*.cpp) \
    $$files(src/undoredo/*.cpp) \
    $$files(src/config/*.cpp) \
    $$files(src/pipeline/*.cpp, true) \
    $$files(ui/*.cpp)

HEADERS += \
    $$files(src/*.h) \
    src/misc/bimaphelper.h \
    $$files(src/model/*.h) \
    $$files(src/command/*.h) \
    $$files(src/undoredo/*.h) \
    $$files(src/config/*.h) \
    $$files(src/pipeline/*.h, true) \
    $$files(ui/*.h)


FORMS += \
    $$files(ui/*.ui)

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

RESOURCES += \
    share/icons/resource.qrc
