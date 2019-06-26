---
id: dataOutput
title: DataOutput
sidebar_label: Data Output
---

## Data output
After a tracking analysis (or an analysis preview), Fast Track saves the tracking data in a file named tracking.txt inside the Tracking_result folder placed in the folder where the image sequence is stored.
* xHead, yHead, tHead: the position (x, y) and the absolute angle of the object's head.
* xTail, yTail, tTail: the position (x, y) and the absolute angle of the object's tail.
* xBody, yBody, tBody: the position (x, y) and the absolute angle of the object.
* headMajorAxisLength, headMinorAxisLength, headExcentricity: parameters of the head's ellipse.
* bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity: parameters of the body's ellipse.
* tailMajorAxisLength, tailMinorAxisLength, tailExcentricity: parameters of the tail's ellipse.
* imageNumber: index of the frame.
* id: object unique identification number.

Note: If several tracking analysis are performed on the same image sequence, the previous folder are not erased, it will be renamed as Tracking_result_DateOfTheNewAnalysis.

## Data analysis

### In Python

The tracking.txt file can be opened in Python to help analyze the data.
```
data = pandas.read_csv("tracking.txt", sep='\t') # Open the data file
subData0 = data[data["id"] == 1]  # Select only the data from object with id 1
subData1 = data[data["imageNumber"] == 150]  # Select only the data from the image 150
```

