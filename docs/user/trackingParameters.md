---
id: trackingParameters
title: Tracking parameters
sidebar_label: Tracking
---

This section details how to select the relevant tracking features to be included in the cost function and how to tune them.

## How it works
FastTrack uses the so-called Hungarian method to solve the assignment problem of each object between two frames. This method is based on minimizing the global cost of the association pairs of objects.

### Cost function
The cost is calculated from a cost function that can be constructed from several parameters. In the following, "i" is indexing the image "n," and "j" indexes the image "n + 1":
* The distance $d_{ij} = \sqrt{(x_i - x_j)^2 + (y_i - y_j)^2}$
* The angle $a_{ij} = \min(\theta_i - \theta_j)$
* The area $ar_{ij} = \lvert area_i - area_j \rvert$
* The perimeter $p_{ij} = \lvert perimeter_i - perimeter_j \rvert$

The relative weight of these contributions to the cost function is set by 4 normalization parameters:
$$
c_{ij} = \frac{d_{ij}}{D} + \frac{a_{ij}}{A}+ \frac{ar_{ij}}{AR} + \frac{p_{ij}}{P}
$$
These parameters can be set to 0 to cancel one or several tracking features from the cost computation. All these features are not always relevant and have to be chosen carefully for the best tracking accuracy. For example, for tracking circles of radius "r" and squares of the same area moving at 10 px/image, it is best to set:
$$
(D = 10, A = 0, AR = 0, P = 2r(\pi-2\sqrt{\pi}))
$$
For tracking fish of the same size, traveling at 35 px/image, doing small reorientation of 20°, it is best to set:
$$
(D = 35, A = 20, AR = 0, P = 0)
$$
For tracking fish of different sizes, traveling at 35 px/image, doing small reorientation of 20°, with a size difference of 100 px, it is best to set:
$$
(D = 35, A = 20, AR = 100, P = 0)
$$ 

The best way to set the parameters is to first set the normalization parameters to the mean of the variable, i.e., the typical change between two consecutive images:
* $D = \text{mean}(d_{ij})$ where "i" and "j" are the same object.
* $A = \text{mean}(a_{ij})$ where "i" and "j" are the same object.
* $AR = \text{mean}(ar_{ij})$ where "i" and "j" are the same object.
* $P = \text{mean}(p_{ij})$ where "i" and "j" are the same object.
In this case, each tracking feature has the same contribution to the cost. To tune the cost function by weighting more (resp. less) a tracking feature, decrease (resp. increase) the normalization parameter of this feature or increase (resp. decrease) all the normalization parameters of the others.

### Memory and distance
A parameter of memory named "maximal time" can be set to account for disappearing objects. If the maximal time is set to "m," one object can only disappear during m images. If it reappears after, it will be considered as a new object.

To speed up the tracking, the maximal distance "L" parameter sets an infinite cost for all the pairs of objects where $d_{ij} > L$. In practice, L corresponds to the maximal distance an object can disappear.

### Spot
The spot to track will determine if the distance and the angular difference will be calculated from the head, the tail, or the body of the object. Area and perimeter are always computed from the body. The head is defined as the bigger half of the object, separated alongside the object's minor axis.

## Conclusion
Setting the tracking parameters can be tedious. It can be best achieved by trials and errors (see the Preview option in the Interactive panel). To summarize:
1. Choose the right tracking features.
2. Set the normalization parameters equal to the tracking feature's standard deviation, i.e., the typical value change.
3. Tune the normalization parameters to increase or decrease the relative weight between each contribution.
