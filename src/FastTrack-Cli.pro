-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T21:49:39
#
#-------------------------------------------------

QT       += core gui network webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fasttrack-cli
TEMPLATE = app

unix:!macx {
  DESTDIR=build_cli
  OBJECTS_DIR=build_cli
  MOC_DIR=build_cli
  UI_DIR=build_cli

  QMAKE_CXXFLAGS += -std=c++17 -O3 -fopenmp -g
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3
  QMAKE_LFLAGS_RELEASE -= -O1
  QMAKE_LFLAGS_RELEASE -= -O2
  QMAKE_LFLAGS_RELEASE += -O3

  INCLUDEPATH += /usr/include/opencv4/
  INCLUDEPATH += /usr/local/include/opencv4/
  LIBS += -L/usr/lib/ -fopenmp
  LIBS += -L/usr/local/lib -L/usr/lib -L/usr/local/lib64 -L/usr/lib64 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_video -lopencv_calib3d -lopencv_photo -lopencv_features2d
  RCC_DIR=build
}

win32 {
  QMAKE_CXXFLAGS += /std:c++latest
  LIBS += -LC:\opencv_build\install\x86\vc15\lib -lopencv_core410d -lopencv_imgproc410d -lopencv_highgui410d -lopencv_imgcodecs410d -lopencv_videoio410d -lopencv_video410d -lopencv_calib3      d410d -lopencv_photo410d -lopencv_features2d410d
  INCLUDEPATH += C:\opencv\build\include
  DEPENDPATH += C:\opencv\build\include
}

unix:macx {
  CONFIG += c++17
  QMAKE_CXXFLAGS += -std=c++1z -stdlib=libc++ -mmacosx-version-min=10.15  -O3 -g
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3

  QMAKE_LFLAGS_RELEASE -= -O1
  QMAKE_LFLAGS_RELEASE -= -O2
  QMAKE_LFLAGS_RELEASE += -O3
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

  QT_CONFIG -= no-pkg-config
  CONFIG  += link_pkgconfig
  PKGCONFIG += opencv4

  ICON=assets/icon.icns
}

SOURCES += \
        fasttrack-cli.cpp \
        tracking.cpp \
        videoreader.cpp \
        Hungarian.cpp \


HEADERS += \
        tracking.h \
        videoreader.h \
        Hungarian.h \

