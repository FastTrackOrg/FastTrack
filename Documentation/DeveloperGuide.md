# Fishy Tracking developer guide

[TOC]



## Introduction

Fishy Tracking is developed to be embedded in existing C++ projects, or to be adapted for a specific use.

## Adapts Fishy Tracking for our project

To adapts Fishy Tracking for our project with a specific workflow, you must implement the **startProcess()** and **imageAnalysis()** method from the Tracking class with our own image analysis workflow.

### startProcess() method

The **startProcess()** method initialize the tracking process by taking the first frame of the image sequence, detects its format and the objects it contains. 

By default the image analysis workflow is the following:

- Read image
- Binarize (optional)
- Thresholding
- Dilate (optional)
- The region of interest selection (optional)
- Object detection (detection + parameters extraction)
- Parameters saving.

The **startProcess()** method will emit a signal with images to display and a signal to trigger the analysis of the rest of the image sequence.

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



## Embedded Fishy Tracking in our project

### Video tracking

To embedded Fishy Tracking in an existing project, you must first create a thread where the Tracking class will live.

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

Fishy Tracking supports live tracking analysis. Be sure to test the program on a video before to see if the analysis frame rate is lower or equal to the tracking analysis frame rate.

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