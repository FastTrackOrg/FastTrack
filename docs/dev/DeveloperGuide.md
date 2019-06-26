# Fast Track developer guide

[TOC]



## Introduction

Fast Track is developed to be embedded in existing C++/Qt projects. It can also be adapted to every project by re-implementing the existing software.

## Installation of the development environment

### Windows

#### Qt installation

1. Go to https://download/qt.io/archive/qt and choose the last version of Qt (this example is made with the 5.12 version). Download the Window installer.

   ![Qt download page](ressources/Qt_Rep.png)

2. Follow the installation steps and select **C:\Qt\Qt5.12.0** for the installation folder. In the Select Components page, select MinGW 7.3.0 64 bit and Qt creator 4.8.

   ![Qt installer dialogue](ressources/Qt_Dial.png)

3. Add MinGW to the path:

   - Open the **Settings** dialogue.

     ![Windows settings dialogue](ressources/Setting_Dial.png)

   - Open the **Edit the system environment variables** and click on the **Environment Variables** button.

     ![Windows Environment Variables dialogue](ressources/Var_Dial.png)

   - Double click on **Path** and add the MinGW path: **C:\Qt\Qt5.12.0\Tools\mingw730_64\bin**.

     ![Windows Edit environment variable dialogue](ressources/Path_Dial.png)

#### OpenCV installation

- Download the last version of OpenCV at https://sourceforge.net/projects/opencvlibrary/files/ (in this example 4.0.1). Select the Windows file **opencv-4.0.1-vc14_vc15.exe**.

- Extract OpenCV in the **C:\ ** folder.

  ![OpenCV self-extracting archive dialogue](ressources/Opencv_Dial.png)

- Download and install CMake https://cmake.org/download/.

- Open CMake an set **Where is the source code** to **C:/opencv/source ** and **Where to build the binaries** to **C:/opencv/source/build **.

  ![CMake dialogue](ressources/1_Cmake.png)

- Click on the Configure button and select **MinGW Makefiles** and tick **Specify native compiler**.

  ![CMake dialogue](ressources/1.5_Cmake.png)

- Set the C and C++ path compiler to **C:/Qt/Qt5.12.0/Tools/mingw730_64/bin/gcc.exe** and **C:/Qt/Qt5.12.0/Tools/mingw730_64/bin/g++.exe**.

  ![CMake dialogue](ressources/3_Cmake.png)

- Untick  **WITH_OPENCL** and tick **WITH_OPENMP** and click on the Configure button. Then click the Generate button.

  ![CMake dialogue](ressources/4_Cmake.png)

- Open the **Command Prompt** application.

  ![Windows Command Prompt application](ressources/Command_Dial.png)

- Type (replace 8 by your the number of processors on your computer) the command

  ```
  cd C:\opencv\sources\build & mingw32-make -j 8 & mingw32-make install
  ```

  to compile and install OpenCV.

- Add  **C:\opencv\sources\build\install\x64\mingw\bin** to the Path.

  ![](ressources/Path2_Dial.png)


#### Configure FastTrack

Replace **FastTrack.pro** by:

```bash
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FastTrack
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
        tracking.cpp \
        setupwindow.cpp \
        Hungarian.cpp

QMAKE_CXXFLAGS += -std=c++11 -O3 -fopenmp

INCLUDEPATH += C:/opencv/build/include


LIBS += C:/opencv/sources/build/install/x64/mingw/bin/libopencv_core401.dll
LIBS += C:/opencv/sources/build/install/x64/mingw/bin/libopencv_highgui401.dll
LIBS += C:/opencv/sources/build/install/x64/mingw/bin/libopencv_imgcodecs401.dll
LIBS += C:/opencv/sources/build/install/x64/mingw/bin/libopencv_imgproc401.dll
LIBS += C:/opencv/sources/build/install/x64/mingw/bin/libopencv_videoio401.dll


HEADERS += \
        mainwindow.h\
        tracking.h \
        setupwindow.h \
        Hungarian.h

FORMS += mainwindow.ui \
        setupwindow.ui 

RESOURCES += resources.qrc

```

### Linux / MacOs

- Download OpenCV.

  ```
  git clone https://github.com/opencv/opencv
  ```

- Compile OpenCV (can need additional dependencies like **build-essential** and **libgl1-mesa-dev**).

  ```bash
  cd opencv
  mkdir build
  cd build
  cmake -D WITH_TBB=ON -D WITH_OPENMP=ON -D WITH_IPP=ON -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_EXAMPLES=OFF -D WITH_NVCUVID=ON -D WITH_CUDA=ON -D BUILD_DOCS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D WITH_CSTRIPES=ON -D WITH_OPENCL=OFF CMAKE_INSTALL_PREFIX=/usr/local/ ..
  make -j8
  sudo make install
  ```

- Compile FastTrack 

  ```bash
  cd FastTrack
  ./run full                // Compiles from scratch
  ./run partial             // Compiles changes
  ./run debug               // Compiles for debugging
  ./run profile            // Creates profiling.txt
  ```




## Adapt Fast Track for our project

To adapt Fast Track for our project, you must re-implement the **startProcess()** and **imageAnalysis()** method from the Tracking class with our own image analysis workflow.

### startProcess() method

The **startProcess()** method initializes the tracking process by taking the first image of the sequence, detects its format and all the objects in the image. 

By default the image analysis workflow is the following:

- Read image
- Binarize (optional)
- Thresholding
- Dilate (optional)
- The region of interest selection (optional)
- Object detection (detection + parameters extraction)
- Parameters saving.

The **startProcess()** method will emit a signal with the images to display and a signal to trigger the analysis of the rest of the image sequence.

### imageAnalysis() method

The **imageAnalysis()** method detects objects, extracts its parameters and associates objects to keep track of individual identity.

By default the image analysis workflow is the following:

- Read image
- Binarize (optional)
- Thresholding
- Dilate (optional)
- The region of interest selection (optional)
- Object detection (detection + parameters extraction)
- Objects association.
- Parameters saving.

The **imageAnalysis()** method will emit a signal with images to display and triggered via a timer the analysis of the next image of the image sequence.



## Embedded Fast Track in our project

### Video tracking

To embedded Fast Track in an existing project, you must first create a thread where the Tracking class will live.

```C++ 
thread = new QThread; // Creates a new QThread
tracking = new Tracking("path/to/folder/where/is/stored/the/image/sequence"); // Instantiates Tracking class
tracking -> moveToThread(thread); // Moves the Tracking instance in the new QThread

connect(thread, &QThread::started, tracking, &Tracking::startProcess); // Starts the tracking analysis when the thread start

// Do here all useful connect like updating parameters, display images etc...

connect(tracking, &Tracking::finished, thread, &QThread::quit); // Shut down the thread when the tracking analysis is finished 
connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater); // Deletes the Tracking instance when the tracking analysis is finished
connect(thread, &QThread::finished, thread, &QThread::deleteLater); // Thread will be deleted only after it has fully shut down 
        
thread->start(); // Starts the tracking analysis
```

The tracking analysis will be running in the thread and destroy itself at the end.

### Real-time tracking (not fully supported in 4.0.0 version)

Fast Track supports live tracking analysis. Be sure to test the program on a video before to see if the analysis frame rate is lower or equal to the tracking analysis frame rate.

Creates an acquisition image thread with an object Camera that sends a signal newImage(UMat) when a new image is available. 

```c++
thread = new QThread; // Creates a new QThread
tracking = new Tracking(""); // Instantiates Tracking class
tracking -> moveToThread(thread); // Moves the Tracking instance in the new QThread

connect(thread, &QThread::started, tracking, &Tracking::startProcess); // Starts the tracking analysis when the thread start

connect(camera, &Camera::newImage, tracking, &Tracking::receivedFrame);
// Do here all usefull connect like updating parameters, display images etc...

connect(tracking, &Tracking::finished, thread, &QThread::quit); // Shut down the thread when the tracking analysis is finished 
connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater); // Deletes the Tracking instance when the tracking analysis is finished
connect(thread, &QThread::finished, thread, &QThread::deleteLater); // Thread will be deleted only after it has fully shut down 
        
thread->start(); // Starts the tracking analysis
```

Note: real-time tracking is not fully supported and tested in version 4.0.0. It will be supported in the 4.1.0 version.

## Generate documentation

The documentation can be generated in HTML and PDF format with Doxygen. Install Doxygen and run

```
./generateDocumentation.sh
```



## Test

FastTrack have a test script that allows to test the code after changes. To use the test script, install gcode.

```
./test
```



Revised 2019/01/10
