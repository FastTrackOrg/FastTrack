# Fast Track

Fast Track is a cross-platform application designed to track multiple objects in video recording. Stable versions of the software are available for Linux, Mac and Windows.

Two main features are implemented in the software:
* An automatic tracking algorithm that can detect and track objects (conserving the objects identities across the video recording).
* A manual review of the tracking where errors can be corrected rapidly and easily by hand.

![alt text](docs/user/assets/interactive_detec.gif)

## Installation stable version

[Installation instructions](docs/user/installation.md)

## Installation development version

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

## Contributing

[Development environment](docs/dev/DeveloperGuide.md)

[Guidelines](contributing.md)


- Website: http://www.fasttrack.sh/
- User manual: http://www.fasttrack.sh/UserManual/docs/intro.html
- Developer guide: http://www.fasttrack.sh/API/index.html
- Bug tracker: http://assistance.fasttrack.sh/

