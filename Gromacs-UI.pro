QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/preferencesdialog.cpp \
    src/project.cpp \
    src/projectmanager.cpp \
    src/step.cpp

HEADERS += \
    src/mainwindow.h \
    src/preferencesdialog.h \
    src/project.h \
    src/projectmanager.h \
    src/step.h

FORMS += \
    ui/mainwindow.ui \
    ui/preferencesdialog.ui

TRANSLATIONS += \
    share/Gromacs-UI_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations
