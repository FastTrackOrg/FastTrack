greaterThan(QT_MAJOR_VERSION, 5): QT += widgets core gui network svg sql

TEMPLATE = app
include("../FastTrack.pri")
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QTPLUGIN += QSQLITE

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

TRANSLATIONS = \
            ../translations/fasttrack_en.ts
qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_VAR_OBJECTS_DIR}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

unix:!macx {
  TARGET = fasttrack

  QMAKE_CXXFLAGS += -std=c++17 -O3
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3
  QMAKE_LFLAGS_RELEASE -= -O1
  QMAKE_LFLAGS_RELEASE -= -O2
  QMAKE_LFLAGS_RELEASE += -O3

  INCLUDEPATH += /usr/include/opencv4/
  INCLUDEPATH += /usr/local/include/opencv4/
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
  translation.files = $$DESTDIR/fasttrack_en.qm
  INSTALLS += target desktop metainfo icon license
}

win32 {
  TARGET = FastTrack
  CONFIG += c++2a
  QMAKE_CXXFLAGS += -O3
  LIBS += -L"$$PWD/../OpenCV_MinGW_64/lib" -lopencv_world4100
  INCLUDEPATH += "$$PWD/../OpenCV_MinGW_64/include"
}

unix:macx {
  TARGET = FastTrack
  CONFIG += c++2a
  QMAKE_CXXFLAGS += -std=c++1z -stdlib=libc++ -O3 -Xpreprocessor
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -O3

  QMAKE_LFLAGS_RELEASE -= -O1
  QMAKE_LFLAGS_RELEASE -= -O2
  QMAKE_LFLAGS_RELEASE += -O3
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/

  QT_CONFIG -= no-pkg-config
  CONFIG  += link_pkgconfig
  PKGCONFIG += opencv4

  ICON=assets/icon.icns
}

SOURCES += \
        updater.cpp \
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
        imageviewer.cpp \


HEADERS += \
        updater.h \
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
        imageviewer.h \


FORMS += \
        mainwindow.ui \
        replay.ui \
        trackingmanager.ui \
        batch.ui \
        interactive.ui \
        timeline.ui \

RESOURCES += resources.qrc \
        assets/theme.qrc
