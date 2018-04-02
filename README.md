# Fishy tracking software

## Installation for Linux

1. Download the lastest release FishyTracking.AppImage at https://github.com/bgallois/FishyTracking/releases/
2. Set as executable as a program by taping chmod a+x FishyTracking.AppImage
3. Execute ./FishyTracking

## Installation for Window

1. Download the lastest release FishyTracking.zip at https://github.com/bgallois/FishyTracking/releases/
2. Unzip FishyTracking.zip
3. Open the folder FishyTracking and execute Fishy.exe

## Installation for Mac
1. Not supported.


# Quick start

## Setting parameters

1. Path: path to the image sequence to analyze, must be '/path/to/my/folder/*.extension'. If the path is correct, the field will become green. In case of error, it will be red.
2. Number of objects: number of objects to track, can't be changed during the analysis.
3. The maximal area of the object: maximal area in pixels of the object, for the best detection, it should be set a little bit bigger than the actual size of the object.
4.  The minimal area of the object: minimal area in pixels of the object, for the best detection, it should be set a little bit smaller than the actual size of the object.
5.  Binary threshold: the threshold value selected to compute the binary mask. It should be adjusted during the tracking for a better result.
6.  Maximal displacement (only for multiple targets): maximal displacement between to frame of one object. A coarse-grained approximation is sufficient. It should be adjusted in live during the tracking for better results.
7.  Maximal angle (only for multiple targets): maximal change of orientation between to frame of one object. A coarse-grained approximation is sufficient. It should be adjusted in live during the tracking for better results.
8.  Maximum occlusion distance: distance that one object can travel when hidden by another object. A coarse-grained approximation is sufficient and the best results are achieved by overvalued this parameter. It should be adjusted in live during the tracking for better results.
9.  Save to: path to the output text file. By default it will be in '/path/to/my/folder/tracking.txt'.
10.  Top corner x position for the ROI: ROI top corner value in pixel. Can be adjusted in live.
11.  Top corner x position for the ROI: ROI top corner x-value in pixel. Can be adjusted in live. If the value is set to zero, the default x-value will be the width of the input image.
12.  Top corner y position for the ROI: ROI top corner y-value in pixel. Can be adjusted in live.
12.  Top corner y position for the ROI: ROI top corner y-value in pixel. Can be adjusted in live. If the value is set to zero, the default value will be the height of the input image.
13.  Spot to track: location of the tracked point of the animal. Can be false for animals that are to symmetrical in the antero-posterior axis or inverted for animal with a tail larger than the head.
14.  Background images: number of image selected to compute the background by averaging the n image taken randomly in the image sequence.

## Errors handling

If an error occurs during the tracking, an error message will be prompted and the parameter field to be adjusted will become red.

### "No files found"

The software can't find any image with the extension specified. The field Path is red.
**Solution**: reset the program and changed the path, the field Path is green if the program can find image in the folder.

### "No fish detected, change parameters"

The software can't detected fish at the start of the analysis.
**Solution**: 
1. Take a look a the binary image, in general the binary threshold is not well adjusted and no object are detected.
2. Change the binary threshold value.
3. Press the Go button.
4. Repeat

### "The ROI does not fit the image size or there is no object in the image. Please try changing the ROI and the minimal area of the object."

The ROI is larger than the image size. 
**Solution**: change the ROI size or set all ROI parameters to zero to auto set the ROI and press the Play button.

No object in the image. This error occurs typically at the start of the analysis.
**Solution**: change the minimal and maximal size of the object and press the Play button, if the error occurs at the start of the analysis press the Reset button.

### "Too many objects in the image that indicated in parameters, try to increase the number of objects or to increase the minimal area of an object."

Occurs at the start of the analysis when to many object are detected (almost always the binary threshold that are not well adjusted). Can occurs during analysis if the background changes abruptly.
**Solution**:
1. Change the binary threshold value.
2. Press the GO button.
3. Repeat.

### Other error message

Error that are not supported by the software, you can send the error at benjamin.gallois@upmc.fr to help the development.


# Not documented tricks

## Frame by frame analysis.
1. Press the Pause button during the analysis.
2. Press the Go button, this will display the tracking frame by frame.

## Impossible tracking (software crash without showing image)
The software shows an error message after clicking the Go button without showing the first frame.

**Solution**: try to increase the object number to see if you can successfully start the analysis and see what is causing the error.  




# WIP

##Parameters

1. Auto setting of maximal and minimal area of the object.
2. Binary threshold: not tested and adjusted for white object on black background.
3. Spot to track: automatically find head or tail for different type of animal.

