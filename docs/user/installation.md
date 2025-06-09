---
id: installation
title: Installation
sidebar_label: Installation
---

![installation](assets/installing.png)

---
**NOTE**

During the installation on Windows and Mac systems, security alerts may be displayed due to the absence of an EV code signing certificate for the FastTrack executable. These alerts can be safely ignored. To verify the FastTrack executable, you can easily and freely compare the MD5 checksum. For more detailed instructions, you can refer to the [installation video](https://www.youtube.com/watch?v=EvfCIS7BmSM).

---

## Download

Stable versions of FastTrack are released for Linux (official makeself, official AUR for ArchLinux), Mac (as app and homebrew), and Windows (installer, portable folder).

## Installation

1.  For Windows:
    * Integrate into the system:
        -   Download the FastTrack [installer FastTrackInstaller.exe](https://github.com/FastTrackOrg/FastTrack/releases/latest).
        -   Execute the installer and follow the provided instructions.
    * Portable:
        - Download the FastTrack [folder FastTrack.zip] (https://github.com/FastTrackOrg/FastTrack/releases/latest).
        - Unzip the folder and execute FastTrack.exe.
2. For Linux (all distributions) as makeself:
    * Download the AppImage [file FastTrack-x86_64.run](https://github.com/FastTrackOrg/FastTrack/releases/latest).
    * Allow FastTrack.run to be executed:
        - Right-click on the AppImage file.
        - Click on Properties.
        - Click on Permissions.
        - Tick “Allow executing file as program”.
3. For Linux natively:
    * Arch Linux: `yay -S fasttrack fasttrack-cli`
    * From source : `mkdir build ; cd build ; cmake ../ ; make ; sudo make install`
4.  For Mac (.app):
    - Minimal version required based on Qt6 minimal version required.
    - Download the FastTrack dmg [FastTrack.dmg](https://github.com/FastTrackOrg/FastTrack/releases/latest).
    - Double click on the dmg file.
    - Drag the application from the dmg window into the Applications folder.
5.  For Mac (brew):
    - Install https://brew.sh/.
    - `brew tap fasttrackorg/fasttrack`
    - `brew install fasttrack`
    - FastTrack-cli can be located at `/usr/local/bin/FastTrack-Cli` and directly used and FastTrack opened using `open /usr/local/Cellar/fasttrack/6.3.1_1/bin/FastTrack.app`.

## macOS Troubleshooting for FastTrack Installation

Follow these troubleshooting steps to ensure a seamless installation and optimal operation of FastTrack on macOS:

### Application Authorization

- **Unsigned Application Warning**: If you encounter a message stating that "FastTrack" is damaged and can't be opened, it's because the application is not signed and will not be signed ever.
  - **Solution**:
    1. Open the Terminal application.

    2. Run the following command to remove the quarantine attribute:

       - For macOS 15 or higher:

         ```bash
         xattr -d com.apple.quarantine /Applications/FastTrack.app
         ```

       - For macOS 14 or lower:

         ```bash
         xattr -dr com.apple.quarantine /Applications/FastTrack.app
         ```

    3. Navigate to `System Settings` > `Privacy and Security` > `Accessibility`.

    4. If `FastTrack` is listed, ensure it is enabled.

    5. If you see an entry named `fasttrack` (without the capital 'F'), enable it as well. If not present:

       - Open Finder and go to `/Applications/FastTrack.app/Contents/MacOS/`.

       - Drag and drop the `fasttrack` binary into the Accessibility settings window.

       - Ensure both `FastTrack` and `fasttrack` are enabled.

## Update

FastTrack will display a message at the start-up and in the status bar when a new release is available.

1. For Windows:  
  Search the *FastTrackUpdater* in the *Windows Start Menu* or execute the *MaintenanceTool.exe* in the installation folder directly and follow the provided instructions.

2. For Linux:  
  The FastTrack AppImage does not currently support the automatic update. Replace the current AppImage with the latest AppImage released.

3. For Mac:  
    * The FastTrack App does not currently support the automatic update. Replace the current App with the latest App released.
    * The brew package support upgrade using `brew upgrade`.
