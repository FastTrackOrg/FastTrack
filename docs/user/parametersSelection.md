---
id: setParam
title: Tips and Tricks to set parameters
sidebar_label: Parameters
---

## Setting the parameters
They are several tricks that can be used to increase tracking accuracy and select the optimal set of parameters.

### Detection
The detection parameters reject objects that are smaller or bigger than a certain size. To increase the tracking accuracy, we want to reject noise and artifacts, and reject blobs constitute of more of one object. If all the objects are of similar size, these two parameters can be selected easily in four steps:

* Select an image where two objects are in contact forming a single blob.
* Select the **Maximal Size** parameter just at the limit to reject this blob.
* Select the **Maximal Size** parameter just at the limit to detect the smaller object of the movie.
* Fine tune these parameters to account for size variability across the movie.

### Tracking
Tracking parameters are mostly found by trials and errors. But some rules of thumbs can be applied. 

**Spot to track** has to be set to **Body** for quasi-symmetric objects and low-resolution objects. For deformable objects with enough resolution, select **Head** or **Tail** according to the part that predicts best the traveling direction of the object.

## Parameters file
For each tracking analysis, FastTrack will save the parameters used in `cfg.toml` that can be reloaded in the software or in fasttrack_cli.
Before FastTrack version 5.2.1, the software used to saved the parameters in `parameter.param`, these file can be converted as following (left: old file, right: new file):

```
                                      title = "FastTrack cfg""

                                      [parameters]
Light background = 0                  lightBack = 0
Maximal size = 170                    maxArea = 170
Maximal occlusion = 200               maxDist = 200
Maximal time = 100                    maxTime = 100
Background method = 1                 methBack = 1
Minimal size = 50                     minArea = 50
Morphological operation = 8           morph = 8
Kernel type = 2                       morphSize = 2
Kernel size = 0                       morphType = 0
Number of images background = 20      nBack = 20
Maximal angle = 90                    normAngle = 90
Binary threshold = 60                 normArea = 0
Normalization area = 0                normDist = 100
Maximal length = 100                  normPerim = 0
Normalization perimeter = 0           reg = 0
Registration = 0                      regBack = 0
Background registration method = 0    spot = 0
Spot to track = 0                     thresh = 60
ROI bottom x = 0                      xBottom = 0
ROI top x = 0                         xTop = 0
ROI bottom y = 0                      yBottom = 0
ROI top y = 0                         yTop = 0
```
