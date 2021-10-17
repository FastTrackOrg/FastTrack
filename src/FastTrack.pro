greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webenginewidgets core gui network webenginewidgets
NO_WEB {
        QT -= webenginewidgets
        DEFINES += NO_WEB
}

TEMPLATE = app
VERSION = 6.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"


unix:!macx {
  TARGET = fasttrack
  
  DESTDIR=build
  OBJECTS_DIR=build
  MOC_DIR=build
  UI_DIR=build
  RCC_DIR=build

  QMAKE_CXXFLAGS += -std=c++17 -O3 -fopenmp -g -Wconversion
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

  # custom destination INSTALL_ROOT=$PWD/package_root make install
  target.path = /usr/bin/
  desktop.path = /usr/share/applications/
  desktop.files = sh.fasttrack.fasttrack.desktop
  metainfo.path = /usr/share/metainfo/
  metainfo.files = sh.fasttrack.fasttrack.metainfo.xml
  icon.path = /usr/share/icons/
  icon.files = src/assets/fasttrack.png
  license.path = /usr/share/licenses/fasttrack/
  license.files = LICENSE
  INSTALLS += target desktop metainfo icon license
}

win32 {
  TARGET = FastTrack
  CONFIG += c++2a
  LIBS += -LC:\opencv\build\x64\vc15\lib -lopencv_world450
  INCLUDEPATH += C:\opencv\build\include
  DEPENDPATH += C:\opencv\build\include
}

unix:macx {
  TARGET = FastTrack
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

  DESTDIR=build
  OBJECTS_DIR=build
  MOC_DIR=build
  UI_DIR=build
  RCC_DIR=build

  QT_CONFIG -= no-pkg-config
  CONFIG  += link_pkgconfig
  PKGCONFIG += opencv4

  ICON=assets/icon.icns
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        tracking.cpp \
        Hungarian.cpp \
        replay.cpp \
        batch.cpp \
        interactive.cpp \
        data.cpp \
        annotation.cpp \
        trackingmanager.cpp \
        videoreader.cpp \
        timeline.cpp \ 
        autolevel.cpp \ 


HEADERS += \
        mainwindow.h\
        tracking.h \
        Hungarian.h \
        replay.h \
        batch.h \
        interactive.h \
        data.h \
        annotation.h \
        trackingmanager.h\
        videoreader.h \
        timeline.h \ 
        autolevel.h \ 


FORMS += \
        mainwindow.ui \
        replay.ui \
        trackingmanager.ui \
        batch.ui \ 
        interactive.ui \ 
        timeline.ui \ 

RESOURCES += resources.qrc \
        assets/theme.qrc

