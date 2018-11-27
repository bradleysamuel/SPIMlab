QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SPIMlab

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    version.cpp \
    logwidget.cpp \
    logger.cpp \
    nidevice.cpp \
    centralwidget.cpp

HEADERS += \
    mainwindow.h \
    version.h \
    logwidget.h \
    logger.h \
    nidevice.h \
    centralwidget.h

RESOURCES += \
    ../resources.qrc

WITH_HARDWARE {
    DEFINES += WITH_HARDWARE
    LIBS += -lnidaqmx
}

CONFIG(debug) {
    DISTFILES += \
        ../.astylerc \
}
