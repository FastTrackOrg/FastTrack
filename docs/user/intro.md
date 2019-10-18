---
id: intro
title: Getting Started
sidebar_label: Getting Started
---


Welcome to the user manual of FastTrack. This page is still under construction, some information can be missing, contact benjamin.gallois@fasttrack.sh for more information.

Fast Track is a cross-platform application designed to track multiple
objects in video recording. Stable versions of the software are
available for Linux, Mac and Windows. The development version and the
source code can be downloaded at https://git.gallois.cc/bgallois/FastTrack.

Two main features are implemented in the software:

-   An automatic tracking algorithm that can detect and track objects
    (conserving the objects identities across the video recording).

-   A manual review of the tracking where errors can be corrected
    rapidly and easily by hand.

What video quality is required?

Fast Track is designed to work with any video quality and framerate. Therefore, FastTrack does not use object features to identify each object. Fast Track uses object direction and position to solve an optimization problem and find the best assignment set possible between objects on two consecutive frames.

Should I use Fast Track?

You can use Fast Track...
- If you don't have complex occlusion events.
- If you have complex occlusion events and don't care to correct manually few errors.
- If you don't care about identities and just want the parts of trajectories between occlusions. 

You can't use Fast Track...
- If you want to maintain the object's identity with complex occlusion events without manually reviewing the tracking.
