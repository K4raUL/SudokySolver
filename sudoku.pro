#-------------------------------------------------
#
# Project created by QtCreator 2016-09-26T11:28:24
#
#-------------------------------------------------

QT       += core gui printsupport sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ololomodel.cpp

HEADERS  += mainwindow.h \
    ololomodel.h

FORMS    += mainwindow.ui
target.path = $$[QT_INSTALL_EXAMPLES]/sql/tablemodel
INSTALLS += target
