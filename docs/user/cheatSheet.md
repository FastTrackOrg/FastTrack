---
id: cheatSheet
title: Parameters cheat sheet
sidebar_label: Parameters cheat sheet
---

This cheat sheet features the parameters that the user have to tune in the software for easy reference.

# Image options

These options are used by the software to process the image and detect the objects.

## Background

* Method: the method used to compute the background by z-projection.
* Number of images: how many images to project to compute the background, these images are selected at a regular interval in the stack.
* Registration: register the image before the z-projection.

## Registration

* Registration: method used to register the image before processing.

## Threshold

* Type: select the type of background (light or dark).
* Value: value selected to threshold the image.

## Morphological operation

* Kernel size: the size of the kernel of the operation.
* Operation: type of operation to perform.
* Kernel type: the shape of the kernel.

## Detection

* Maximal size: maximal size in pixels of the detected object.
* Minimal size: minimal size in pixels of the detected object.

# Image options

* Normalization distance (legacy Maximal length/Typical distance): average distance travels in pixels by an object between two successive images.
* Normalization angle (legacy Maximal angle/Typical angle): average change in direction in degrees of an object between two successive images.
* Maximal distance: maximal distance for assigning an object between two successive images. If an object travels more than this distance, it will be assigned a new id.
* Maximal time: maximal time, in frame number, that an object can disappear. If an object is not detected in more images than this time, a new id is assigned.
* Spot to track: what ellipse (head, tail or body) parameters are used for the tracking.
