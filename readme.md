# FastTrack

 ![Test](https://github.com/bgallois/FastTrack/workflows/Tests/badge.svg) ![Clang Format](https://github.com/bgallois/FastTrack/workflows/Clang%20Format/badge.svg) ![Continous Build](https://github.com/bgallois/FastTrack/workflows/Continous%20Builds/badge.svg) [![Gitter chat](https://badges.gitter.im/gitterHQ/gitter.png)](https://gitter.im/fasttracksh) [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Website](https://img.shields.io/website?down_color=red&down_message=offline&style=plastic&up_color=green&up_message=online&url=http%3A%2F%2Fwww.fasttrack.sh)](http://www.fasttrack.sh)

## About

Tracking objects in video recording can be tedious. Existing softwares are expensive, the learning curve are step, and the workflow can not be easily adapted when the project evolved.

**FastTrack** is a tracking desktop software, easy to install and easy to used. The software is open-source and can be downloaded and modified to adapt any needs.
Two main features are implemented in the software:
* An automatic tracking algorithm that can detect and track objects (conserving the objects identities across the video recording).
* A manual review of the tracking where errors can be corrected rapidly and easily by hands to achieved 100% accuracy.

![alt text](docs/user/assets/interactive_detec.gif)

## Installation

Stable builds of the software are available for Linux, Mac and Windows on the [download page](http://www.fasttrack.sh/UserManual/docs/installation/). Nightly builds are available on the [releases page](https://github.com/bgallois/FastTrack/releases).

## Development environment

For Debian based distribution:
```
sudo apt-get install build-essential cmake mesa-common-dev
sudo apt-get install qt5-default 
cd fasttrack
./dependencencies.sh opencv
./run.sh full
```

For Arch based distribution:
```
sudo pacman -S cmake gcc
sudo pacman -S qt5-base
cd fasttrack
./dependencencies.sh opencv
./run.sh full
```

For Windows and Mac see [the developer guide](http://www.fasttrack.sh/API/index.html)

## Contributing

[Development environment](docs/dev/DeveloperGuide.md)

[Guidelines](contributing.md)

- Website: http://www.fasttrack.sh/
- User manual: http://www.fasttrack.sh/UserManual/docs/intro.html
- Developer guide: http://www.fasttrack.sh/API/index.html

