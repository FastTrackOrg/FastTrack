---
id: intro
title: Getting Started
sidebar_label: Getting Started
---


Welcome to the FastTrack user manual. This manual will present the tracking software and how to use it. Please contact Benjamin Gallois by email at **benjamin.gallois@fasttrack.sh** if you need more information or to signal a bug. If you encounter any problem, please check at [FastTrack issues](https://github.com/FastTrackOrg/FastTrack/issues) to see if the error is already signaled and being addressed. For comments or suggestions, please open a [discussion](https://github.com/FastTrackOrg/FastTrack/discussions).

FastTrack is a cross-platform application designed to track multiple objects in video recording. Stable versions of the software are available for Linux, Mac, and Windows. The source code can be downloaded at https://github.com/FastTrackOrg/FastTrack.

Two main features are implemented in the software:

-  An automatic tracking algorithm that can detect and track objects, conserving the objects' identities across the video recording.
-  An ergonomic tool allows the user to check, correct and annotate the tracking.

The FastTrack user interface is implemented with Qt and the image analysis with the OpenCV library. This allows a performant and responsive software amenable to processing large video recordings. FastTrack uses SQLite database to store the data internally and tracking results are exported in a plain text file (as well as accessible through the database).

FastTrack was first a [Ph.D. thesis](https://hal.archives-ouvertes.fr/tel-03243224/document) side project started by [Benjamin Gallois](https://github.com/bgallois) in his spare time that has then taken dedicated time in his Ph.D. project. The software's core is still maintained in his spare time; therefore, new features implementation, bug fixes, and help can take some time.

**Not sure if you want to use FastTrack? Check these five most common questions:**

**What video quality is required?**
FastTrack is designed to work with any video quality and frame rate.

**What type of objects and numbers can FastTrack handled?**
<iframe id="lbry-iframe" width="560" height="315" src="https://lbry.tv/$/embed/FastTrack-demo/b23ab74d4a632261ebd2bf4286e5ff7460395616" allowfullscreen></iframe>

**How does it perform?**
Tracking performances depend on systems (number and type of objects). However, with the built-in ergonomic tool, it is possible to achieve 100% tracking accuracy with minimal effort.

**It is free?**
FastTrack is a [free](https://www.gnu.org/philosophy/free-sw.en.html) software under the [GPL3 license](https://www.gnu.org/licenses/gpl-3.0.en.html).

**Do I need programming skills?**
No.
