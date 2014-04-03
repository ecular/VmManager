#-------------------------------------------------
#
# Project created by QtCreator 2014-03-18T10:44:57
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VmManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newvm.cpp \
    confirmdialog.cpp \
    xmloperator.cpp \
    virshcmd.cpp \
    getvirtinfo.cpp \
    settingdialog.cpp

HEADERS  += mainwindow.h \
    newvm.h \
    confirmdialog.h \
    xmloperator.h \
    virshcmd.h \
    getvirtinfo.h \
    timestruct.h \
    settingdialog.h

FORMS    += mainwindow.ui \
    newvm.ui \
    confirmdialog.ui \
    settingdialog.ui

RESOURCES += \
    iconImage.qrc

 LIBS += -lvirt
