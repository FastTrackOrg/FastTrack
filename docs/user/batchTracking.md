---
id: batchTracking
title: Batch Tracking
sidebar_label: Batch Tracking
---

The Batch Tracking panel provides a means to analyze a large number of files easily.

## Basic use

You can open multiple movies that you want to analyze by clicking on the open folder button and selecting one or several folders. If a background image or a parameter file is available (from previous analysis), it can be automatically loaded if the Auto Load Background and Parameters option is ticked. This option is useful if you have already tuned the tracking parameters and previewed the tracking with the Interactive panel.
If you want to manually set up the parameters, you have to set the parameters in the Parameter Table *before* opening the movie.
You can also load a background image or a parameters file by clicking on the buttons in the Path Table after having opened a folder.

## More advanced options

### Default behavior

If the Auto Load Background and Parameters option is unticked, and if the user doesn't select any background or parameters file, the analysis will proceed with the set of parameters selected in the Parameters Table *at the time* of the file opening.

### Add a suffix

You can add a suffix to the selected folders. It will add "/suffix" to each folder path.
For example if you have a folder tree like this:
- /myExperiment/Run1/image
- /myExperiment/Run2/image
- /myExperiment/Run3/image

You can easily select the folders:
- /myExperiment/Run1
- /myExperiment/Run2
- /myExperiment/Run3

With the dialog, and then add the suffix "image" to select the desired folder without having to do it manually.

### One background image for all the analysis

You can add a unique background image for each analysis. Open an image with the "Unique background" button and all the waiting analysis and analysis that will be added by the user will be using this background image. You can use the clear button at the right of the background image path to reset the default behavior.

### One parameter file for all the analysis

* Untick the Auto Load Background and Parameters option, choose the right set of parameters with the Parameters Table and the analysis that will be added will use this parameter set.
* You can also tick the Auto Load Background and Parameters option, if a parameter file exists, that will load this parameter set in the Parameter Table. Then untick the Auto Load Background and Parameters option and all the analysis that will be added will use the parameter set present in the Parameter Table.
test
