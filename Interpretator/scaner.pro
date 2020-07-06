QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        Anallizator.cpp \
        Scanner.cpp \
        Tree.cpp \
        main.cpp

HEADERS += \
    Anallizator.h \
    Dictionary.h \
    Scanner.h \
    Tree.h
