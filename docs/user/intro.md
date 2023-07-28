---
id: intro
title: Getting Started
sidebar_label: Getting Started
---

![getting_started](assets/getting_started.png)

Welcome to the FastTrack user manual. This manual will introduce you to the tracking software and provide instructions on how to use it. If you need further information or want to report a bug, please contact Benjamin Gallois via email at **benjamin.gallois@fasttrack.sh**. In case you encounter any issues, we recommend checking the  [FastTrack issues](https://github.com/FastTrackOrg/FastTrack/issues)  page to see if the problem has already been reported and is currently being addressed. For any comments or suggestions, please feel free to participate in the [discussion](https://github.com/FastTrackOrg/FastTrack/discussions). Your feedback is valuable to us!

FastTrack is a versatile cross-platform application specifically designed for tracking multiple objects in video recordings. The software offers stable versions for Linux, Mac, and Windows operating systems. For those interested, the source code is available for download at https://github.com/FastTrackOrg/FastTrack.

The software boasts two primary features:

1. An automatic tracking algorithm capable of detecting and tracking objects while preserving their identities throughout the video recording.
2. An ergonomic tool that enables users to review, correct, and annotate the tracking results.

With these features, FastTrack provides a comprehensive solution for efficient and accurate object tracking in videos.

The FastTrack user interface is implemented with Qt, and image analysis is performed using the OpenCV library. This combination enables the software to deliver excellent performance and responsiveness, making it well-suited for processing large video recordings. To store data internally, FastTrack utilizes an SQLite database, while tracking results are exported in plain text format and are also accessible through the database.

FastTrack originated as a side project during  [Benjamin Gallois](https://github.com/bgallois)' Ph.D. thesis, which you can find [here](https://hal.archives-ouvertes.fr/tel-03243224/document) . Over time, it evolved into a dedicated part of his Ph.D. project. Despite being a spare-time endeavor, Benjamin continues to maintain the software's core. As a result, the implementation of new features, bug fixes, and support may take some time to be addressed. We appreciate your understanding and patience in this regard.

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

**I need more flexibility?**
PyFastTrack is a Python library that integrates the tracking technology of FastTrack. With PyFastTrack, you can combine FastTrack with your own trained YOLO detector, allowing you to detect and track various objects with ease.
