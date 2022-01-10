---
id: installation
title: Installation
sidebar_label: Installation
---


---
**NOTE**

During the installation on Windows and Mac systems, security alerts are displayed because the FastTrack executable does not possess an EV code signing certificate.  These alerts can be ignored. FastTrack executable can be verified easily (and freely) by comparing the MD5 checksum.

---

## Download
Stable versions of FastTrack are released for Linux, Mac (as dmg), and Windows (installer). The nightly version is available on the [Github Actions artifacts](https://github.com/FastTrackOrg/FastTrack/actions).

## Installation

1.  For Windows:
    -   Download the FastTrack [installer](https://fasttrack.sh/download/FastTrackInstaller.exe).
    -   Execute the installer and follow the provided instructions.
    -   If FastTrack is complaining about missing DLL, go to the installation folder and execute the vc_redist.x64.exe executable.
2. For Linux (all distributions) as AppImage:
    * Download the AppImage [file](https://fasttrack.sh/download/FastTrack-x86_64.AppImage).
    * Allow FastTrack.AppImage to be executed:
        - Right click on the AppImage file.
        - Click on Properties.
        - Click on Permissions.
        - Tick “Allow executing file as program”.
    * Check the [AppImage](https://appimage.org/) Launcher to integrate AppImage into the system.
3. For Linux natively:
    * Fedora: `sudo dnf install fasttrack fasttrack-cli`
    * Arch Linux: `yay -S fasttrack fasttrack-cli`
    * From source : `qmake src/FastTrack.pro ; make ; sudo make install ; qmake src/FastTrack-Cli.pro ; make ; sudo make install`
4.  For Mac:
    - Minimal version required: 10.15.
    - Download the FastTrack dmg [file](https://fasttrack.sh/download/FastTrack.dmg).
    - Double click on the dmg file.
    - Drag the application from the dmg window into the Applications folder.

## Update
FastTrack will display a message at the start-up when a new release is available.

1. For Windows:  
  Search the *FastTrackUpdater* in the *Windows Start Menu* or execute the *MaintenanceTool.exe* in the installation folder directly and follow the provided instructions.

2. For Linux:  
  The FastTrack AppImage does not currently support the automatic update. Replace the current AppImage with the latest AppImage released.

3. For Mac:  
  The FastTrack App does not currently support the automatic update. Replace the current App with the latest App released.
