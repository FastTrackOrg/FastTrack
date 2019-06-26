---
id: dataPreparation
title: Data Preparation
sidebar_label: Data Preparation
---

Fast Track can only analyze image sequences[^1]. The images must follow the
zeroes padding naming convention (name00000.extension,
name00001.extension, name00002.extension …). “pgm”, “png”, “jpeg”,
“jpg”, “tiff”, “tif”, “bmp”, “dib”, “jpe”, “jp2”, “webp”, “pbm”, “ppm”,
“sr”, “ras”, “tif” image formats are supported.

If you have a recording in a video format, you need to convert it into
an image sequence:

1.  For Linux, Windows, and MacOs in command lines:

    -   Install ffmpeg.
    -   In a terminal type: ffmpeg -i video.webm image-%06d.png

2.  For Linux, Windows, and MacOs with a  graphical interface:

    -   Use [Handbrake](https://handbrake.fr/).
    -   Use [ImageJ](https://imagej.nih.gov/ij/download.html).

[^1]: Movies (.avi, .mkv etc...) will be supported in the version 4.10.
