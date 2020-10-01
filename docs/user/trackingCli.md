---
id: trackingCli
title: FastTrack Command line interface
sidebar_label: Tracking CLI
---

A command line interface is available for MacOs, Linux and by using WSL for Windows. It can be downloaded on [the release page](https://github.com/FastTrackOrg/FastTrack/releases/tag/v5.1.5).

The full list of parameters can be found by calling ```./fasttrack-cli --help```. 
Parameters can be declared individually by calling ```./fasttrack-cli --path path/to/movie.webm --parameter1 value --parameter2 value``` or in batch with a parameters file ```./fasttrack-cli --path path/to/movie.webm --cfg path/parameters.param```. Note that the path option need to be the first option.

## Calling FastTrack from a Python script

FastTrack can be called inside a Python script to automate tracking. 

```
import os

cmd = "./fasttrack-cli --maxArea 500 --minArea 50 --lightBack 0 --thresh 80 --reg 0 --spot 0 --nBack 50 --regBack 0 --methBack 1 --xTop 0 --yTop 0 --xBottom 0 --yBottom 0 --morph 0 --morphSize 0 --morphType 0 --normaArea 0 --normPerim 0 --normDist 50 --normAngle 45 --maxDist 150 --maxTime 10 --path ZFJ_001.avi --backPath dataSet/images/Groundtruth/Tracking_Result/background.pgm " 
os.system(cmd)
```

```
import os

cmd = "./fasttrack-cli --path ZFJ_001.avi --cfg Tracking_Result_ZFJ_001/parameters.param"
os.system(cmd)
```
