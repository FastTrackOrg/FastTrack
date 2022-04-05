---
id: interactiveTracking
title: Interactive Tracking
sidebar_label: Interactive Tracking
---

The Interactive panel provides a way to perform a tracking analysis and review it in an interactive environment.
Several steps have to be performed in the right order (some are mandatory, some are optional) to perform a successful tracking analysis.
![Workflow](assets/interactive_workflow.svg)

## Opening a file

The first step of a tracking analysis is to open a video file. FastTrack supports video files and image sequences. Click on the file or an image of a sequence to automatically load the movie.
![File opening](assets/interactive_open.gif)

## Computing the background

The background can be computed or imported. To compute the background, select a method and an image number. Images are selected in the image sequence at regular intervals, and three methods of computation by z-projection are available: 

* Min: each pixel of the background image is the pixel with the minimal value across the selected images from the image sequence. Useful when the objects are light on a dark background.
* Max: each pixel of the background image is the pixel with the maximal value across the image sequence's selected images. Useful when the objects are dark on a light background.
* Average: each pixel of the background image is the average of the pixels across the image sequence's selected images.

The images can be registered before the z-projection. Three methods of registration are available.
![Background computing](assets/interactive_back.gif)

## Selecting a region of interest (optional)

To select a region of interest, draw a rectangle  on the display with the mouse and click on the Crop button. Cancel the crop by clicking on the reset button.
![Cropping](assets/interactive_crop.gif)

## Computing the binary image

To compute the binary image from the background image and the image sequence, select the threshold value, and see the result on the display. The background type is automatically selected after the background computation. However, it can be modified: select Dark Background if the objects are light on a dark background, and Light background if the objects are dark on a light background.
![Binarizing](assets/interactive_thresh.gif)

## Applying morphological operations (optional)

It is possible to apply a morphological operation on the binary image. Select a morphological operation, kernel size, and geometry. See the result on the display. For more information about the different operations, see https://docs.opencv.org/trunk/d9/d61/tutorial_py_morphological_ops.html.
![Applying morphological operations](assets/interactive_morph.gif)

## Tuning the detection parameters

Objects are detected by their size. Select the maximum and minimum size of the detected objects. The detected objects will be colored in green in the display, and the rejected object will be displayed in red.
![](assets/interactive_detec.gif)

## Tuning the tracking parameters

Several parameters can be modified to ensure a good tracking analysis. See [this page](http://www.fasttrack.sh/docs/trackingParameters/) for more details:

### Hard parameters

Hard parameters have to be set manually by the user:

* Maximal distance: if an object traveled more than this distance between two consecutive images, it would be considered as a new object.
* Maximal time: number of images an object is allowed to disappear. If an object reappears after this time, it will be considered as a new object. If the number of objects is constant throughout the movie, set the Maximal Time equal to the movie's number of frames.
* Spot to track: part of the object features used to do the tracking. Select the part that reflects the better the direction of the object. Legacy parameter, head corresponds to the smaller mid-part of the object, tail ellipse the wider mid-part of the object, and body is the full object.

### Soft parameters

The soft parameters can be leveled automatically by clicking on the Level button. This will automatically compute the soft parameters as each contribution weighs one quarter of the total cost (see more at [this page](https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1008697#sec003) section "automatic tracking parameters"). It has to be manually fine-tuned by the user to find the optimal soft parameters with the system's knowledge. For example, for a system where the objects' direction is not relevant, the user will select the Normalization angle equal to 0.

* Normalization distance (legacy Maximal length/ Typical length): typical distance traveled between two consecutive images.
* Normalization angle (legacy Maximal angle/Typical angle): typical reorientation possible between two consecutive images.
* Normalization area: typical difference in the area.
* Normalization perimeter: typical difference in the perimeter.

## Registration

The image registration is the process to correct small displacements and rotations of the camera during the movie recording. FastTrack provides several methods for registering the movie: 

* By phase correlation
* ECC image alignment
* Features based

Image registration is very computationally intensive and can drastically decrease the speed of the program.

## Previewing the tracking

The tracking can be previewed on a sub-sequence of images. It can be useful to tune parameters if the tracking is slow.

## Display options

Several display options are available and unlocked at each step of the analysis.

* Original: original image sequence
* Background subtracted: image sequence minus the background image.
* Binary: binary image sequence with detection overlays.
* Tracking: tracking data overlay. 

## Layout options

Several layouts and themes are available in the layout menu in the top bar. The user can also build his layout by dragging the option docks in the window.

[See a video demonstration](https://www.youtube.com/watch?v=grxAAX0J6CQ&feature=youtu.be)
