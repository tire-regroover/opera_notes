#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T21:04:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opera_notes
TEMPLATE = app


SOURCES += main.cpp\
        opera_notes.cpp \
    opera_notes_parser.cpp

HEADERS  += opera_notes.h \
    opera_notes_parser.h \
    version.h

FORMS    += opera_notes.ui

RESOURCES += opera_notes.qrc
LIBS += -lboost_regex
unix {
    isEmpty(PREFIX):PREFIX = /usr/local
    binaries.path = $$PREFIX/bin
    binaries.files = $$TARGET
    INSTALLS += binaries
}

OTHER_FILES += \
    opera_notes.png \
    opera_notes.desktop
