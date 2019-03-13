#-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T21:49:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FastTrack
TEMPLATE = app

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE += -O3


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        tracking.cpp \
        Hungarian.cpp \
        replay.cpp \
        batch.cpp \
        interactive.cpp \
        data.cpp \

QMAKE_CXXFLAGS += -std=c++11 -O3 -fopenmp -g

INCLUDEPATH += /usr/local/include/opencv4/ /usr/local/include/quazip/
INCLUDEPATH += /usr/local/include/quazip/
LIBS += -L/usr/local/lib64/ -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_video -lopencv_photo -fopenmp
LIBS += -L/usr/local/lib/ -lquazip
HEADERS += \
        mainwindow.h\
        tracking.h \
        Hungarian.h \
        replay.h \
        batch.h \
        interactive.h \
        data.h \


FORMS += \ 
        mainwindow.ui \
        replay.ui \ 
        batch.ui \ 
        interactive.ui \ 

RESOURCES += resources.qrc

