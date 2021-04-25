#-------------------------------------------------
#
# Project created by QtCreator 2020-08-12T10:02:10
#
# Jincheng Bai(BaiBai)
# Modify 2020.12.15
#-------------------------------------------------

QT       += core gui dbus
QT       += x11extras KWindowSystem
RC_FILE += mypng.rc
OTHER_FILES += mypng.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TRANSLATIONS    += ./translations/kylin-recorder_zh_CN.ts
QT += multimedia multimediawidgets
TARGET = kylin-recorder
TEMPLATE = app
LIBS += -lmp3lame

# 生成qm文件
message($$system(./translations/generate_translations_qm.sh))
# translations
#TRANSLATIONS += $$PWD/translations/kylin-recorder_zh_CN.ts
QM_FILES_INSTALL_PATH = /usr/share/kylin-recorder/translations/
CONFIG += lrelease
# 适配窗口管理器圆角阴影
LIBS +=-lpthread
LIBS +=-lX11

qm_files.files = translations/*.qm
qm_files.path = /usr/share/kylin-recorder/translations/

target.path = /usr/bin
target.source +=$$TARGET

desktop.files = kylin-recorder.desktop
desktop.path = /usr/share/applications/

appdesktop.files += kylin-recorder.desktop
appdesktop.path = /usr/share/applications/

icons.files = svg/recording_128.svg
icons.path = /usr/share/pixmaps/

# gsettings
schemes.files += \
    $$PWD/data/org.kylin-recorder-data.gschema.xml \
    $$PWD/data/org.ukui.log4qt.kylin-recorder.gschema.xml
#schemes.files = data/org.kylin-recorder-data.gschema.xml
schemes.path = /usr/share/glib-2.0/schemas/

LIBS += -L/usr/lib/libukui-log4qt.so.1.0.0 -lukui-log4qt


INSTALLS += target desktop appdesktop icons  schemes qm_files

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += gsettings-qt

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/clipbutton.cpp \
        src/daemondbus.cpp \
        src/itemswindow.cpp \
        src/lineedit.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/menumodule.cpp \
        src/miniwindow.cpp \
        src/mp3encoder.cpp \
        src/mythread.cpp \
        src/mywave.cpp \
        src/save.cpp \
        src/settings.cpp \
        src/tipwindow.cpp \
        src/xatom-helper.cpp

HEADERS += \
        src/clipbutton.h \
        src/daemondbus.h \
        src/itemswindow.h \
        src/lineedit.h \
        src/mainwindow.h \
        src/menumodule.h \
        src/miniwindow.h \
        src/mp3encoder.h \
        src/mythread.h \
        src/mywave.h \
        src/save.h \
        src/settings.h \
        src/tipwindow.h \
        src/xatom-helper.h

RESOURCES += \
    picture.qrc

DISTFILES += \
    data/org.kylin-recorder-data.gschema.xml \
    data/org.ukui.log4qt.kylin-recorder.gschema.xml \
    mypng.rc \
    translations/kylin-recorder_zh_CN.qm \
    translations/kylin-recorder_zh_CN.ts \
    translations/qt_zh_CN.qm
