---
id: installation
title: Installation
sidebar_label: Installation
---

---
**NOTE**

During the installation on Windows and Mac systems, security alerts are displayed due to the fact that the FastTrack executable does not possess an EV code signing certificate. You can ignore these alerts. FastTrack executable can be verified easily (and freely) by comparing the MD5 checksum.
* 0e721fd36644edc72de202afc8a9144c  FastTrack.AppImage
* 48fce2c287111f2301a1d086965297e6  FastTrack.dmg
* bfa9ffea9ea459203c8f396de224a485  FastTrackInstaller.exe 

---

## Download
Stable versions of FastTrack are released for Linux (as AppImage and Ubuntu 18 PPA), Mac
(as dmg) and Windows (installer). The development version is accessible from source and can be compiled (please refer to the developer guide at http://www.fasttrack.sh/API/index.html).

## Installation
1.  For Windows:
    -   Download the FastTrack [installer](/download/FastTrackInstaller.exe).
    -   Execute the installer and follow the provided instructions.

2. For Linux (all distributions) as AppImage (no system integration):
    * Download the AppImage [file](/download/FastTrack.AppImage).
    * Allow FastTrack.AppImage to be executed:
    -   Right click on the AppImage file.
    -   Click on Properties.
    -   Click on Permissions.
    -   Tick “Allow executing file as program”.
    * Check the [AppImage](https://appimage.org/) Launcher to integrate .appImage to the system.
    
3. For Ubuntu 18.04 (automatic update and system integration):
    ```
    wget -O - https://www.fasttrack.sh/download/apt/conf/public.gpg.key | sudo apt-key add - 
    echo "deb https://www.fasttrack.sh/download/apt/ sid main" | sudo tee -a /etc/apt/sources.list
    sudo apt-get update 
    sudo apt-get install fasttrack
    ```

5.  For Mac:
    -   Download the FastTrack dmg [file](/download/FastTrack.dmg).
    -   Double click on the dmg file.
    -   Drag the application from the dmg window into the Applications
        folder.

## Update
FastTrack will display a message at the start-up when new a release is available.

1. For Windows:
Search the *FastTrackUpdater* in the *Windows Start Menu* or execute directly the *MaintenanceTool.exe* in the installation folder and follow the provided instructions.

2. For Linux as AppImage:
The FastTrack AppImage currently not support the automatic update. Replace the current AppImage with the latest AppImage released.

3. For Ubuntu 18:
```
sudo apt-get update
sudo apt-get dist-upgrade
```

4. For Mac
The FastTrack App currently not support the automatic update. Replace the current App with the latest App released.
