greaterThan(QT_MAJOR_VERSION, 5): QT += widgets core gui

TARGET = Test
TEMPLATE = app
CONFIG += testcase
CONFIG += no_testcase_installs

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build


QMAKE_LFLAGS_RELEASE += -O3

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        TrackingTest.cpp \
        ../src/tracking.cpp \
        ../src/videoreader.cpp \
        ../src/Hungarian.cpp \
        ../src/autolevel.cpp \
        ../src/data.cpp \

QMAKE_CXXFLAGS += -std=c++17 -O3 -fopenmp -g

INCLUDEPATH += /usr/include/opencv4/
INCLUDEPATH += /usr/local/include/opencv4/ /usr/include/gtest
LIBS += -L /usr/local/lib64/  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio  -lopencv_calib3d -lopencv_photo -lopencv_features2d -lopencv_photo -lopencv_video -fopenmp -lgtest -lgtest_main
HEADERS += \
        ../src/tracking.h \
        ../src/videoreader.h \
        ../src/Hungarian.h \
        ../src/autolevel.h \
        ../src/data.h \
        /usr/include/gtest/gtest.h \


