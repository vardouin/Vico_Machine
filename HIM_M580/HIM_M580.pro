QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QT += widgets network
SOURCES += \
    btn.cpp \
    custombutton.cpp \
    diode.cpp \
    main.cpp \
    mainwindow.cpp \
    message.cpp \
    sender.cpp \
    textreceiver.cpp \
    textsender.cpp

HEADERS += \
    btn.h \
    custombutton.h \
    diode.h \
    mainwindow.h \
    message.h \
    sender.h \
    textreceiver.h \
    textsender.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    sender.ui
