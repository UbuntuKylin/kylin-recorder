#-------------------------------------------------
#
# Project created by QtCreator 2020-08-12T10:02:10
#
#-------------------------------------------------

QT       += core gui
RC_FILE += mypng.rc
OTHER_FILES += mypng.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += multimedia multimediawidgets
TARGET = kylin-recording
TEMPLATE = app

target.path = /usr/bin
target.source +=$$TARGET

desktop.files = kylin-recording.desktop
desktop.path = /usr/share/applications/

appdesktop.files += kylin-recording.desktop
appdesktop.path = /usr/share/applications/

icons.files += /png/png/recording_32.png
icons.path = /usr/share/pixmaps/

INSTALLS += target desktop appdesktop icons

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        lineedit.cpp \
        main.cpp \
        mainwindow.cpp \
        mythread.cpp \
        mywave.cpp \
        save.cpp \
        settings.cpp

HEADERS += \
        lineedit.h \
        mainwindow.h \
        mythread.h \
        mywave.h \
        save.h \
        settings.h

RESOURCES += \
    picture.qrc

DISTFILES += \
    mypng.rc
