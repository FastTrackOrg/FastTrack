---
id: trackingCli
title: FastTrack Command line interface
sidebar_label: Tracking CLI
---

## Installing fasttrack-cli
A command-line interface is available for macOS, Linux, and by using WSL for Windows. It can be downloaded on [the release page](https://github.com/FastTrackOrg/FastTrack/releases/).

The full list of parameters can be found by calling ```./fasttrack-cli --help```. 
Parameters can be declared individually by calling ```./fasttrack-cli --path path/to/movie.webm --parameter1 value --parameter2 value``` or in batch with a parameters file ```./fasttrack-cli --path path/to/movie.webm --cfg path/cfg.toml```. Note that the path option need to be the first option.

fasttrack-cli does not support Windows natively. The workaround is to use WSL.
* Install WSL https://docs.microsoft.com/en-us/windows/wsl/install-win10.
* Install FastTrack in a Linux terminal:
```
wget https://github.com/FastTrackOrg/FastTrack/releases/download/continuous_cli/fasttrack-cli-x86_64.AppImage
chmod +x fasttrack-cli-x86_64.AppImage
./fasttrack-cli-x86_64.AppImage --appimage-extract
sudo ln -s ~/squashfs-root/usr/bin/fasttrack-cli /usr/local/bin/
```
* FastTrack-cli can now be called directly by typing fasttrack-cli --help 

## Calling FastTrack from a Python script

FastTrack can be called inside a Python script to automate the tracking. 

```
import os

cmd = "./fasttrack-cli --maxArea 500 --minArea 50 --lightBack 0 --thresh 80 --reg 0 --spot 0 --nBack 50 --regBack 0 --methBack 1 --xTop 0 --yTop 0 --xBottom 0 --yBottom 0 --morph 0 --morphSize 0 --morphType 0 --normArea 0 --normPerim 0 --normDist 50 --normAngle 45 --maxDist 150 --maxTime 10 --path ZFJ_001.avi --backPath dataSet/images/Groundtruth/Tracking_Result/background.pgm " 
os.system(cmd)
```

```
import os

cmd = "./fasttrack-cli --path ZFJ_001.avi --cfg Tracking_Result_ZFJ_001/cfg.toml"
os.system(cmd)
```
