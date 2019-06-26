---
id: trackingInspector
title: Tracking Inspector
sidebar_label: Tracking Inspector
---

The tracking Inspector allows the user to display the result of a tracking analysis, and to manually correct the tracking if necessary.

## Load an image sequence
Click on the open folder button or copy the path to the folder where the image sequence is stored in the text field, available tracking data will be automatically loaded.
Click on the reload button to reload the tracking data.

## Display options
Several display options are available:
* Ellipse: display the head, tail and or body ellipses on the tracked objects.
* Arrows: display an arrow on the head, tail and or body of the tracked object indicating the orientation.
* Numbers: display the ids of the tracked objects.
* Traces: display the previous 50 positions of the tracked objects.
* Size: the size of the tracking overlay.
* Frame rate: display frame rate (this option is indicative, the maximal frame rate is set by the computer performance and image size).

## Inspect the tracking
The tracking can be inspected by moving the display cursor and the bottom of the panel. Automatically detected occlusions can be reviewed by clicking on the previous and next occlusion button at the top right of the display.

## Correct the tracking
### Swap the data of two objects
To correct an occlusion by swapping two the data for two objects from the current image to the end of the sequence:
* Left click on the first object, the object ID and color is displayed on the first box at the top left of the display.
* Right click on the first object, the object ID and color is displayed on the second box at the top left of the display.
* Right-click or click on the Swap button to exchange the data for the two selected objects from the current image to the last image.
### Delete the data of an object
To delete the data for one object from the current image to the end of the sequence:
* Left click on the object, the object ID and color is displayed on the first box at the top left of the display.
* Click on the delete button to delete the data from the current image to the last image.

## Export a movie
To export a movie of a tracking analysis, select the desired display overlay and click on the save button. Select a folder and a name to save the file, only .avi format is supported.
