---
id: trackingInspector
title: Tracking Inspector
sidebar_label: Tracking Inspector
---
---

**NOTE**

The Tracking Inspector panel is accessible in Expert Mode (settings -> Expert Mode), or in the Interactive panel (View -> Tracking replay).

---


![alt text](assets/tracking_inspector.svg)

* 1: Open
* 2: Open Tracking_Result directory
* 3: Reload
* 4: Export video
* 5: Undo
* 6: Redo
* 8: First object selection box
* 9: Swap
* 9: Second object selection box
* 10: Delete one
* 11: Delete
* 12: Delete selection box
* 13: Previous/Next occlusion (experimental)
* 14: Annotation search
* 15: Annotation previous match
* 16: Annotation next match
* 17: Annotation entry
* 18: Objects information table
* 19: Timeline
* 20: Image number
* 21: Play
* 22: FPS selection box
* 23: Display
* 24: Overlay

**The Tracking Inspector** is a tool designed to display the results of a tracking analysis and to enable manual corrections when necessary. For instance, users can delete an object to remove artifacts or change the object ID to correct tracking errors. FastTrack provides an ergonomic interface with built-in keyboard shortcuts, making the user's experience more seamless. By alleviating the tedious work of review and correction, FastTrack allows users to achieve 100% tracking accuracy rapidly and efficiently.

## Loading a tracking analysis

To load a previously tracked analysis in FastTrack, follow these steps:

1. Click on the **Open** button (1) and select either a movie or an image sequence. This will load the latest available tracking analysis.
If the movie was tracked multiple times, the last tracking analysis is stored in the **Tracking_Result** folder, while the previous tracking analyses are stored in **Tracking_Result_Date** folders.
2. To load an older tracking analysis, use the **Open Tracking_result directory** button (2) (this button is only activated if a movie is already loaded).
3. If necessary, click on the **Reload** button (3) to refresh the tracking data.

Note: The software can only load a tracking analysis if the folder architecture is preserved. In other words, the folder containing the image sequence or movie must have a sub-folder named **Tracking_Result**, which must contain at least the *tracking.txt* file.

## Display options

Several tracking overlay options are available on the tracking overlay panel (24):

* **Ellipse**: Display the head, tail, and/or body ellipses on the tracked objects.
* **Arrows**: Display an arrow on the head, tail, and/or body of the tracked object indicating the orientation.
* **Numbers**: Display the IDs of the tracked objects.
* **Traces**: Display the previous 50 positions of the tracked objects.
* **Size**: Adjust the size of the tracking overlay.
* **Frame rate**: Display and save frame rate.

Additionally, several useful pieces of information about the selected object can be found in the information table (18). The user can directly click on the corresponding cell in the table to navigate to the image where the object first appeared.

## Inspecting the tracking

The tracking can be inspected by moving the display cursor (19), observing the image number (20), and automatically playing the movie (21) at a selected frame rate (22).

Automatically detected occlusions (overlapped objects) can be reviewed by clicking on the **Previous** (12) and **Next** (13) occlusion buttons. Please note that this function is experimental and may miss some occlusions.

## Annotating the tracking

The user can annotate any image of the tracking by writing the annotation in the annotate text entry (17). Annotations can be searched across using the find bar (14) and the buttons (15)(16). All annotations are saved in the *annotation.txt* file in the **Tracking_Result** folder.

## Correcting the tracking

### Swapping the data of two objects

The user can correct an error by swapping the IDs of two objects from the current image with the end of the sequence, as follows:

1. Left-click on the first object; the object ID and color will be displayed in the first selection box (6).
2. Left-click on the second object; the object ID and color will be displayed in the second selection box (8).
3. Right-click or click on the **Swap Button** (7) to exchange the IDs of the two selected objects, moving them from the current image to the last image of the sequence.

### Delete the data of an object

To delete one object from several frames:

1. Double left-click on the object; the object ID and color will be displayed in the second selection box (8).
2. Select the number of frames over which you want to delete the object in the box (11). Shortcut C is available to focus on the selection box.
3. Click on the **Delete** button (10) to remove the object from the current frame to the current frame plus the selected number.

To delete one object from the current frame:

1. Double left-click on the object; the object ID and color will be displayed in the second selection box (8).
2. Click on the **Delete One** button (9) to remove the object from the current frame.

### Keyboard shortcuts summary

A set of keyboard shortcuts is available to speed up the tracking correction:

- Q/A: go to the previous image.
- D: go to the next image.
- F: delete the selected object on the current image.
- C: enter the number of images where an object has to be deleted.
- G: delete an object from the current image to the current image plus the selected number.

## Saving

All the changes made in the inspector are automatically saved to the original *tracking.txt* file and the SQLite database.

## Exporting a movie

To export a movie of a tracking analysis, follow these steps:

1. Select the desired display overlay in the tracking analysis.
2. Click on the **Save** button (3).
3. Choose a folder and specify a name for the file.
4. Please note that only .avi format is supported for the exported movie.

Note: Movies with many objects per frame can be challenging to load and review in the tracking Inspector.

[See a video demonstration](https://youtu.be/5lhx-r_DHLY)
