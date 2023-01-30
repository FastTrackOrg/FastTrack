greaterThan(QT_MAJOR_VERSION, 5): QT += widgets core gui sql

TARGET = Test
TEMPLATE = app
CONFIG += testcase
CONFIG += no_testcase_installs
CONFIG += console

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

DEFINES += QT_DEPRECATED_WARNINGS

unix:!macx {
  INCLUDEPATH += /usr/include/opencv4/
  INCLUDEPATH += /usr/local/include/opencv4/ /usr/include/gtest
  LIBS += -L /usr/local/lib64/  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio  -lopencv_calib3d -lopencv_photo -lopencv_features2d -lopencv_photo -lopencv_video -lgtest -lgtest_main
  QMAKE_LFLAGS_RELEASE += -O3
  QMAKE_CXXFLAGS += -std=c++17 -O3 -g
}

unix:macx {
  CONFIG += c++17
  QMAKE_CXXFLAGS += -std=c++1z -stdlib=libc++ -mmacosx-version-min=10.15 -O3 -g -Xpreprocessor
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
  QT_CONFIG -= no-pkg-config
  CONFIG  += link_pkgconfig
  PKGCONFIG += opencv4
  PKGCONFIG += gtest
}

win32 {
  CONFIG += c++2a
  QMAKE_CXXFLAGS += -O3 -g
  LIBS += -L"$$PWD/../OpenCV_MinGW_64/bin" -lopencv_world470
  INCLUDEPATH += "$$PWD/../OpenCV_MinGW_64/include"
  LIBS += -L"$$PWD/../googletest/build/install/lib" -lgtest -lgtest_main
  INCLUDEPATH += "$$PWD/../googletest/build/install/include/"
}

SOURCES += \
        TrackingTest.cpp \
        ../src/tracking.cpp \
        ../src/videoreader.cpp \
        ../src/Hungarian.cpp \
        ../src/autolevel.cpp \
        ../src/data.cpp \


HEADERS += \
        ../src/tracking.h \
        ../src/videoreader.h \
        ../src/Hungarian.h \
        ../src/autolevel.h \
        ../src/data.h \
