#-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T21:49:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test
TEMPLATE = app

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build


QMAKE_LFLAGS_RELEASE += -O3

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        TrackingTest.cpp \
        ../src/tracking.cpp \
        ../src/videoreader.cpp \
        ../src/Hungarian.cpp \

QMAKE_CXXFLAGS += -std=c++17 -O3 -fopenmp -g

INCLUDEPATH += /usr/local/include/opencv4/ /usr/include/gtest
LIBS += -L /usr/local/lib64/  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio  -lopencv_calib3d -lopencv_photo -lopencv_features2d -lopencv_photo -lopencv_video -fopenmp -lgtest -lgtest_main
HEADERS += \
        ../src/tracking.h \
        ../src/videoreader.h \
        ../src/Hungarian.h \
        /usr/include/gtest/gtest.h \


