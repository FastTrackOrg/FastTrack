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

**Weight** has to be set to 1 for symmetric objects where the angular dependency is irrelevant.