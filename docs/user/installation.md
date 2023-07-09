---
id: installation
title: Installation
sidebar_label: Installation
---


---
**NOTE**

During the installation on Windows and Mac systems, security alerts are displayed because the FastTrack executable does not possess an EV code signing certificate. These alerts can be ignored. FastTrack executable can be verified easily (and freely) by comparing the MD5 checksum. See the [installation video](https://www.youtube.com/watch?v=EvfCIS7BmSM) for more details.

---

## Download

Stable versions of FastTrack are released for Linux (official AppImage, official AUR for ArchLinux, community Fedora package), Mac (as dmg), and Windows (installer, portable folder). The nightly versions are available on the [download page](https://www.fasttrack.sh/download/Continuous/).

## Installation

1.  For Windows:
    * Integrate into the system:
        -   Download the FastTrack [installer](https://fasttrack.sh/download/FastTrackInstaller.exe).
        -   Execute the installer and follow the provided instructions.
    * Portable:
        - Download the FastTrack [folder] (https://fasttrack.sh/download/FastTrack.zip).
        - Unzip the folder and execute FastTrack.exe.
2. For Linux (all distributions) as AppImage:
    * Download the AppImage [file](https://fasttrack.sh/download/FastTrack-x86_64.AppImage).
    * Allow FastTrack.AppImage to be executed:
        - Right-click on the AppImage file.
        - Click on Properties.
        - Click on Permissions.
        - Tick “Allow executing file as program”.
    * Check the [AppImage](https://appimage.org/) Launcher to integrate AppImage into the system.
3. For Linux natively:
    * Fedora: `sudo dnf install fasttrack fasttrack-cli`
    * Arch Linux: `yay -S fasttrack fasttrack-cli`
    * From source : `qmake src/FastTrack.pro ; make ; sudo make install ; qmake src/FastTrack-Cli.pro ; make ; sudo make install`
4.  For Mac (.app):
    - Minimal version required based on Qt6 minimal version required.
    - Download the FastTrack dmg [file](https://fasttrack.sh/download/FastTrack.dmg).
    - Double click on the dmg file.
    - Drag the application from the dmg window into the Applications folder.
5.  For Mac (brew):
    - Install https://brew.sh/.
    - `brew tap fasttrackorg/fasttrack`
    - `brew install fasttrack`
    - FastTrack-cli can be located at `/usr/local/bin/FastTrack-Cli` and directly used and FastTrack opened using `open /usr/local/Cellar/fasttrack/6.3.1_1/bin/FastTrack.app`.

## Update

FastTrack will display a message at the start-up and in the status bar when a new release is available.

1. For Windows:  
  Search the *FastTrackUpdater* in the *Windows Start Menu* or execute the *MaintenanceTool.exe* in the installation folder directly and follow the provided instructions.

2. For Linux:  
  The FastTrack AppImage does not currently support the automatic update. Replace the current AppImage with the latest AppImage released.

3. For Mac:  
    * The FastTrack App does not currently support the automatic update. Replace the current App with the latest App released.
    * The brew package support upgrade using `brew upgrade`.
