---
id: batchTracking
title: Batch Tracking
sidebar_label: Batch Tracking
---
---

**NOTE**

The Batch Tracking panel is only accessible in Expert Mode (settings -> Expert Mode).

---

![alt text](assets/batchTracking.svg)

* 1: Open folder
* 2: Unique background
* 3: Suffix selection
* 4: Processing stack
* 5: Background selection
* 6: Parameter selection
* 7: Progress bar
* 8: Status
* 9: Parameters table
* 10: Autoload
* 11: Clear background
* 12: Start tracking
* 13: Clear stack
* 14: Remove from stack

The Batch Tracking panel is an advanced tool used to automatically track a large number of movies. It allows you to combine several behaviors to load image sequences in a batch, along with specific background images or parameter files. This powerful feature streamlines the tracking process for multiple movies at once.

## Basic usage

The user can open several image sequences by clicking on the **Open folder** (1) button and selecting one or several folders. FastTrack can automatically load a background and/or a parameters file if a **Tracking_Result** folder is provided within the image sequence; check the **Autoload** (10) tick to activate this behavior.

After opening, image sequences are added to the **Processing stack** (4). If a background image and/or a set of parameters are automatically loaded, their paths will be displayed in the second and third columns, respectively. If not, the user can select them using the (5) and (6) buttons after importing.

**By default**, if no background image and parameter file are selected, FastTrack will use the parameters provided in the Parameters table (9) **before** the image sequence importation.

The user can delete an image sequence by selecting the corresponding line in the **Processing stack** (4) and clicking on the **Remove** (14) button. To clear the entire **Processing stack**, the user can click the **Clear** (13) button.

To process the stack, click the **Start Tracking** (12) button, and FastTrack will perform the tracking analysis on all the image sequences in the stack.

## More advanced options

### Adding a suffix

The user can append a suffix to the imported folders by using the *folder_path/ + suffix/* notation.

For example, it can be useful with a folder tree like this one:

- /myExperiment/Run1/images
- /myExperiment/Run2/images
- /myExperiment/Run3/images

The user can easily select all the folders at once:

- /myExperiment/Run1
- /myExperiment/Run2
- /myExperiment/Run3

And then add the suffix *images/* to select the desired folders without having to do it manually three times. This feature allows for a more efficient and convenient selection of folders.

### Unique background image

The user can select a unique background image by opening an image with the **Unique background** (2) button. Once a background image is selected, **all the sequences in the stack** and any sequences that will be imported afterward will use this selected background image. This means that the chosen background image will be applied to all the sequences during the tracking process.

To reset to the default behavior and remove the selected background image, the user can use the **Clear** (12) button. This allows the user to revert to the original state where each sequence might have its own background image or none at all.

### One parameter file

To apply the same parameters file to all the imported sequences, you have the following options:

Manual selection:

1. Untick the **Autoload** (10) checkbox.
2. Select a set of parameters in the **Parameters table** (9).
3. The sequences that will be imported will use this set of parameters.

With a file:

1. Tick the **Autoload** (10) checkbox.
2. Load the sequence with the right parameters file.
3. Untick the **Autoload** (10) checkbox.
4. The sequences that will be imported will use this set of parameters.

Alternative method with a file:

1. Untick the **Autoload** (10) checkbox.
2. Load a sequence.
3. Select the parameter file using the (6) button.
4. The sequences that will be imported will use this set of parameters.

## Behavior reminder

- (10) unticked, (2) not selected: FastTrack will use the parameters provided in the Parameters table (9) **before** the image sequence is added to the stack. It can be overwritten after importation with the (5) and (6) buttons.

- (10) ticked, (2) not selected: FastTrack will attempt to use the background and the parameters file found in the Tracking_Result folder of the image sequence. If these files are missing, FastTrack will fallback to using the parameters provided in the Parameters table (9) **before** the image sequence is added to the stack.

- (10) ticked, (2) selected: When you select a unique background in (2), it will overwrite the automatically detected background for all the sequences in the stack. The selected background will be used for tracking.

- (3) selected: If you select a suffix in (3), it will be appended to the image sequence path, and the default behavior will be applied using this modified path.

- (2) selected: When you select a unique background in (2), it will overwrite all the existing backgrounds in the stack. The selected background will be applied to all sequences during tracking.
