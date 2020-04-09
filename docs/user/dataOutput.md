---
id: dataOutput
title: Tracking Result
sidebar_label: Tracking Result
---

After a tracking analysis (or an analysis preview), FastTrack saves several files inside the **Tracking_Result** folder located inside the image sequence folder:
* *tracking.txt*: the tracking result
* *annotation.txt*: the annotation
* *background.pgm*: the background image
* *parameters.param*: the parameters used for the tracking

The tracking result file is simply a text file with 20 columns separated by a '\t' character. This file can easily be loaded to subsequent analysis see [this example](http://www.fasttrack.sh/UserManual/blog/2019/06/21/Data-analysis-python/).
* **xHead, yHead, tHead**: the position (x, y) and the absolute angle of the object's head.
* **xTail, yTail, tTail**: the position (x, y) and the absolute angle of the object's tail.
* **xBody, yBody, tBody**: the position (x, y) and the absolute angle of the object.
* **headMajorAxisLength, headMinorAxisLength, headExcentricity**: parameters of the head's ellipse (headMinorAxisLength and headExcentricity are semi-axis length).
* **bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity**: parameters of the body's ellipse (bodyMinorAxisLength and bodyExcentricity are semi-axis length).
* **tailMajorAxisLength, tailMinorAxisLength, tailExcentricity**: parameters of the tail's ellipse (bodyMinorAxisLength and bodyExcentricity are semi-axis length).
* **imageNumber**: index of the frame.
* **id**: object unique identification number.

Positions are in pixels, in the frame of reference of the original image, zero is in the top left corner. Lengths and areas are in pixels. Angles in radians in the intervale 0 to 2*pi.

    0  --  ► x  
    ¦  
    ▼  
    y  

**Note:** If several tracking analysis are performed on the same image sequence, the previous folder is not erased, it will be renamed as **Tracking_result_DateOfTheNewAnalysis**.

## Data analysis

The tracking file can be opened for subsequent analysis:
```python
# Python
data = pandas.read_csv("tracking.txt", sep='\t')
```

```julia
# Julia
using CSV
CSV.read("tracking.txt", delim='\t')
```

```R
# R
read.csv("tracking.txt", header=T sep="\t")
```