# User Manual

[TOC]



## General information

Fishy Tracking is an application that allows the user to track multiple animals in video recording. The application takes a sequence of images and outputs a folder with the tracking data, the parameter used to produce the data and a log file. Fishy Tracking can operate on Linux, Mac, and Windows systems.

## Getting started

### Download

Stable versions of Fishy Tracking are released for Linux (installer or AppImage), MacOs (installer or dmg) and Windows (installer). Development versions are accessible from source and can be compiled for every system.

1. Stable versions
   - Download the installer for your system at http://fishytracking.benjamin-gallois.fr/download.html.
2. Development versions
   - Install OpenCV 4 on your system by following instructions at https://docs.opencv.org/4.0.0/df/d65/tutorial_table_of_content_introduction.html.
   - Install Qt5 on your system by following instructions at http://doc.qt.io/qt-5/gettingstarted.html#offline-installation.
   - Download Fishy Tracking code source at https://git.benjamin-gallois.fr/bgallois/FishyTracking.
   - Referred to the developer user guide.

### Installation

1. With Fishy Tracking installer

   - Executes the installer and follows instructions.

2. With Linux AppImage

   Allows FishyTracking.appimage to be executed:

   - Right click on the AppImage file.
   - Click on properties.
   - Click on permissions.
   - Check allow executing the file as a program.

##  Software overview

### Files panel

The files panel allows the user to add several paths to folders where the images sequence to analyze are stored. Multiple paths can be added to analyze data in a bunch.

1. Add the path in the text bar.
2. Click on the button add path.
3. Remove a path if necessary by selecting it in the path table and by clicking on the button delete path.
4. Start the analysis by clicking on start analysis. 

### Display panel

The display panel shows the current analysis process by displaying the binary image (1) and the tracking image (2). The tracking can be extremely fast, to review the tracking referred to the replay panel.

### Parameters panel

The parameters panel provides a mean to change the tracking parameters before or during analysis. We recommend finding a set of parameters by changing parameters one by one and controlling effects in the display panel. When an optimal set of parameters are found, restart the analysis.
An interactive help to determine some parameters is available by clicking on the help button at the bottom of the panel.

Parameters set are saved and reload at each closing and opening of the program.

| Parameter                   | Range                                        | Description                                                  |
| --------------------------- | -------------------------------------------- | ------------------------------------------------------------ |
| Dilatation                  | 0 to inf                                     | Dilates detected objects                                     |
| Registration                | yes, no                                      | Registers images                                             |
| Light background            | yes, no                                      | yes: dark objects on light background, no: light object on dark background |
| ROI bottom y                | 0 to image height                            | 0: no ROI. y-coordinate of the bottom right corner of the rectangle ROI. |
| ROI bottom x                | 0 to image width                             | 0: no ROI. x-coordinate of the bottom right corner of the rectangle ROI. |
| ROI top y                   | 0 to ROI bottom y                            | 0: no ROI. y-coordinate of the top left corner of the rectangle ROI. |
| ROI top x                   | 0 to ROI bottom x                            | 0: no ROI. x-coordinate of the top left corner of the rectangle ROI. |
| Number of images background | 1 to number of images in the image sequences | Number of images to average to compute the background. The images are taken at equal interval in the images sequence. |
| Arrow size                  | 2 to inf                                     | Size of the arrow displayed on the object in the tracking image in the display panel. |
| Maximal occlusion           | 0 to inf                                     | Maximal distance in pixels that an object can travel when occluded. |
| Weight                      | 0 to 1                                       | Weight between difference in displacement or difference in direction to compute the cost (see matching method). |
| Maximal angle               | 0 to 360                                     | Angle in degree that an object can change between two frames (see matching method). |
| Maximal length              | 0 to inf                                     | Length that an object can travel between two frames (see matching method). |
| Spot to track               | 0, 1, 2                                      | 0: head, 1: tail, 2: body. Body part parameters (position and angle) used for the matching process (see matching method). |
| Binary threshold            | 0 to 255                                     | Threshold value that separate background and foreground.     |
| Minimal size                | 0 to maximal size                            | Minimal size in pixels of the tiniest object to track in the images sequences. |
| Maximal size                | minimal size to inf                          | Maximal size in pixels of the biggest object to track in the images sequences. |
| Object number               | 1 to inf                                     | Number of object to track.                                   |



### Replay panel

The replay panel allows the user to open and see the result of a tracking analysis and to correct the tracking if errors are detected.

#### Correct errors

To correct a tracking error (mismatching between two objects):

1. Get to the frame where the error occurs.
2. Left click on the first object or select the number of the first object in the combo box in the top left. The box displays the number and the color of the selected object.
3. Left click on the second object or select the number of the second object in the other combo box in the top left corner.
4. Right click on the image to swap the two objects from the image displayed to the end of the images sequence, or click on the swap button at the right of the two combo box.

#### Occlusion events review

The tracking error occurs mostly during occlusion events. An occlusion review mode is available by clicking on the next and previous button at the top right of the image. This will display an image where an occlusion event occurs.

#### Correct rapidly a tracked images sequence

Mouse and keyboards shortcuts provide to the user a main to rapidly correct a tracked image sequence.

For AZERTY keyboard:

- Next image: Z
- Previous image: S
- Next occlusion: Q
- Previous occlusion: D
- Play: Space

For QWERTY keyboard:

- Next image: W
- Previous image: S
- Next occlusion: A
- Previous occlusion: D
- Play: Space

Mouse shortcuts:

- Left click to select objects.
- Right click to swap objects.

## Input files

Fishy Tracking analyzes images sequence. "pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif" image formats are supported.

If you have recorded in video format, you need to convert it into images sequence as follow.

1. For Linux, windows, and mac in command lines
   - Install ffmpeg.
   - In a terminal type: ffmpeg -i video.webm image-%03d.png
2. For Linux, windows, and mac in graphical
   - Use Handbrake: https://handbrake.fr/ or ImageJ.

## Output files

When an analysis is started, Fishy Tracking creates a folder named Tracking_Result containing:

- tracking.txt containing the tracking data (in the frame of reference of the full image with the origin in the top left corner) as following:

  | xHead                              | yHead                              | tHead                       | xTail                              | yTail                              | tTail                       | xBody                              | yBody                              | tBody                       | Curvature           | ImageNumber            |
  | ---------------------------------- | ---------------------------------- | --------------------------- | ---------------------------------- | ---------------------------------- | --------------------------- | ---------------------------------- | ---------------------------------- | --------------------------- | ------------------- | ---------------------- |
  | x coordinate of the head. Object 1 | y coordinate of the head. Object 1 | Angle of the head. Object 1 | x coordinate of the tail. Object 1 | y coordinate of the tail. Object 1 | Angle of the tail. Object 1 | x coordinate of the body. Object 1 | y coordinate of the body. Object 1 | Angle of the body. Object 1 | Curvature. Object 1 | Image number. Object 1 |
  | x coordinate of the head. Object 2 | y coordinate of the head. Object 2 | Angle of the head. Object 2 | x coordinate of the tail. Object 2 | y coordinate of the tail. Object 2 | Angle of the tail. Object 2 | x coordinate of the body. Object 2 | y coordinate of the body. Object 2 | Angle of the body. Object 2 | Curvature. Object 2 | Image number. Object 2 |
  | x coordinate of the head. Object 1 | y coordinate of the head. Object 1 | Angle of the head. Object 1 | x coordinate of the tail. Object 1 | y coordinate of the tail. Object 1 | Angle of the tail. Object 1 | x coordinate of the body. Object 1 | y coordinate of the body. Object 1 | Angle of the body. Object 1 | Curvature. Object 1 | Image number. Object 1 |
  | ...                                | ...                                | ...                         | ...                                | ...                                | ...                         | ...                                | ...                                | ...                         | ...                 | ...                    |

  Note: if you want to extract the data for a particular object:
  $$
  data_i = (objectNumber)*imageNumber + objectNumber
  $$
  with objectNumber the object number {1, 2, 3, ...}, imageNumber the image number {0, 1, 2, ...}.

- parameters.txt containing the set of parameters at the beginning of the analysis, changes during the analysis are not saved.

## For developers

A complete API and developer documentation is available. It can be generated for the latest version of Fishy Tracking by executing ./generateDocumentation.sh. A folder Documentation is created with the developer documentation.

## Performance

The performance can vary with the selected parameters. Registration and dilatation can cause a loss of 50% in performance. The number of objects influences the performance as the cube of the object number.



Revised: 2018/12/22







