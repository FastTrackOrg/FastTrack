---
id: trackingCli
title: FastTrack Command line interface
sidebar_label: Tracking CLI
---

## Calling fasttrack-cli

The syntax to call FastTrack command line interface differs according to the platform. Replace the generic `./fasttrack-cli` by the right version:  
`./FastTrack-cli-x86_64.AppImage [OPTION]`  
`FastTrack-Cli.exe [OPTION]`  
`open -w FastTrack-cli.app --args [OPTION]`  
`fasttrack-cli [OPTION]`   

## Installing fasttrack-cli

The command-line interface is available for macOS, Linux, and Windows. Since version 6.5.0 the fasttrack-cli is incorporated into the main fasttrack executable.

The full list of parameters can be found by calling ```./fasttrack --cli --help```. 
Parameters can be declared individually by calling ```./fasttrack --cli --path path/to/movie.webm --parameter1 value --parameter2 value``` or in batch with a parameters file ```./fasttrack-cli --path path/to/movie.webm --cfg path/cfg.toml```. Note that the path option need to be the first option.

## Calling fasttrack cli from a Python script

FastTrack can be called inside a Python script to automate the tracking. 

```
import os

cmd = "./fasttrack --cli --maxArea 500 --minArea 50 --lightBack 0 --thresh 80 --reg 0 --spot 0 --nBack 50 --regBack 0 --methBack 1 --xTop 0 --yTop 0 --xBottom 0 --yBottom 0 --morph 0 --morphSize 0 --morphType 0 --normArea 0 --normPerim 0 --normDist 50 --normAngle 45 --maxDist 150 --maxTime 10 --path ZFJ_001.avi --backPath dataSet/images/Groundtruth/Tracking_Result/background.pgm " 
os.system(cmd)
```

```
import os

cmd = "./fasttrack --cli --path ZFJ_001.avi --cfg Tracking_Result_ZFJ_001/cfg.toml"
os.system(cmd)
```

## Parameters

```
Usage:  [OPTION]... [FILE]...
Use FastTrack from the command line.

All arguments are mandatory except --backPath and --cfg. Loading a configuration file with --cfg overwrite any selected parameters.
  --maxArea                  maximal area of objects
  --minArea                  minimal area of objects

  --lightBack                is the background light? 0: Yes, 1: No
  --thresh                   binary threshold, if lightBack is set to 0 (resp. 1), pixels with values less (resp. more) than thresh are considered to belong to an object
   --reg                     registration method, 0: None, 1: Simple, 2: ECC, 3: Features

  --spot                     part of the object that features is used for the matching, 0: head, 1: tail, 2: body
  --normDist                 normalization distance pixels
  --normAngle                normalization angle degres
  --normArea                 normalization area pixels
  --normPerim            normalization perimeter pixels
  --maxDist                  maximal distance of matching in pixels, if an object travels more than this distance, it is considered as a new object
  --maxTime                  maximal time, if an object disappears more than this time, it is considered as a new object

  --nBack                    number of images to compute the background
  --methBack                 method to compute the background. 0: min, 1: max, 2: average
  --regBack                  registration method to compute the background. 0: None, 1: Simple, 2: ECC, 3: Features

  --xTop                     roi x top corner (0:width-1)
  --yTop                     roi y top corner (0:height-1)
  --xBottom                  roi x top corner (0:xTop-1), set to 0 to avoid cropping
  --yBottom                  roi y top corner (0:yTop-1), set to 0 to avoid cropping

  --morph                    morphological operation, 0: None, 1: Erode, 2: Dilate, 3: Open, 4: Close, 5: Gradient, 6: TopHat, 7: BlackHat, 8: HitMiss
  --morphSize                size of the kernel used in the morphological operation, can be omited if no operation are performed
  --morphType                type of the kernel used in the morphological operation, can be omited if no operation are performed, 0: Rect, 1: Cross, 2: Ellipse

  --path                     path to the movie or one image of a sequence
  --backPath                 optional, path to a background image

  --cfg                      optional, path to a configuration file, if path is not included in the configuration file, --path option need to be put before --cfg option
```
