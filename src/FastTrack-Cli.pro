greaterThan(QT_MAJOR_VERSION, 5): QT += widgets webenginewidgets core gui network

TARGET = fasttrack-cli
TEMPLATE = app
VERSION = 6.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

unix:!macx {
  DESTDIR=build_cli
  OBJECTS_DIR=build_cli
  MOC_DIR=build_cli
  UI_DIR=build_cli
  RCC_DIR=build_cli

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

  target.path = /usr/bin/
  license.path = /usr/share/licenses/fasttrack/
  license.files = LICENSE
  INSTALLS += target license
}

unix:macx {
  CONFIG += c++17
  QMAKE_CXXFLAGS += -std=c++1z -stdlib=libc++ -mmacosx-version-min=10.15  -O3 -g
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

  QMAKE_LFLAGS_RELEASE -= -O1
  QMAKE_LFLAGS_RELEASE -= -O2
  QMAKE_LFLAGS_RELEASE += -O3
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/

  DESTDIR=build_cli
  OBJECTS_DIR=build_cli
  MOC_DIR=build_cli
  UI_DIR=build_cli
  RCC_DIR=build_cli

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

