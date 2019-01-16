# General information

Fast Track is an application that allows the user to track animals in video recording. The application takes a sequence of images and outputs a folder with the tracking data, parameters used to produce the data and a log file. Fast Track can operate on Linux, Mac, and Windows systems.

The software provides to the user an automatic tracking algorithm and a facultative manual review of the tracking where errors can be corrected easily.
Fast Track can be used with the provided software (see the user manual) or integrated into any C++/Qt project (see developer guide).

A complete user manual can be found at http://fishytracking.benjamin-gallois.fr/UserManual/UserManual.html.

A developer guide and API documentation can be found at http://fishytracking.benjamin-gallois.fr/API/index.html.

# Getting started

### Download

Stable versions of Fast Track are released for Linux (AppImage file), MacOs (dmg file) and Windows (installer). Development version is accessible from source and can be compiled for every system.

1. Stable versions
   - Download the installer for your system at http://fishytracking.benjamin-gallois.fr/download.html.
2. Development versions
   - Install OpenCV 4 on your system by following instructions at https://docs.opencv.org/4.0.0/df/d65/tutorial_table_of_content_introduction.html.
   - Install Qt5 on your system by following instructions at http://doc.qt.io/qt-5/gettingstarted.html#offline-installation.
   - Download Fast Track code source at https://git.benjamin-gallois.fr/bgallois/FastTrack.
   - Referred to the developer user guide to compile the project.

### Installation

1. With Fast Track installer:

   - Executes the installer and follows instructions. 

2. With Linux AppImage:

   Allows FastTrack.appimage to be executed:

   - Right click on the AppImage file.
   - Click on properties.
   - Click on permissions.
   - Check "allow executing the file as a program".

3. With MacOs dmg file:

   - Drag the application from the dmg file window into /Applications. 

