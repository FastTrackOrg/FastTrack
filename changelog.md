# FastTrack changelog

## 4.8.3

### Fixed
- Fixed saving error in the Replay panel.
- Fixed global background not working in the Batch panel.
- Minor ui fixes.

### Added
- Better keyboard shortcut in the Replay Panel.
- Button to delete one object on one image in the Replay panel.
- Scale to see the length parameter in the Interactive tracking.
- Automatic refresh in the Replay panel when a parameter is changed.

## 4.8.2

### Changed
- Changed Replay panel ui design.
- Changed Interactive panel ui design.

## 4.8.1

### Changed
- Old tracking data can be opened in the Replay panel.
- Changed ui design.

## 4.8.0

### Fixed
- Id numbering issue.
- Crop not working properly if the rectangle get out of the image.
- Fixed crash if tracking data are corrupted in the Replay panel.

### Changed
- Changed the id display in the Replay Panel, allowing the user to select any Id int the bottom combo box.
- New tracking analysis not overwrite the previous but renames the previous one as Tracking_Result_DateOfTheNewAnalysis.
- An error is triggered if the inputed image background is not openable.

### Added
- New registration method: ECC and feature based.
- Undo/redo capabilities in the Replay panel.

## 4.7.0

### Fixed
- Batch tracking crash on Windows.
- Minor file opening errors.

### Added
- Multi-paths selection in the Batch panel.
- Added layout options in the Interactive panel.

### Changed
-  Batch panel ui.

## 4.6.1

### Fixed
- Batch tracking crash on Windows.

### Added
- Multi-paths selection in the Batch panel (beta).

## 4.6.0

### Added
- Progress bar for the background computation in the Interactive panel.
- Automatic background type in the Interactive panel.
- New morphological operation and morphological elements.
- Progress bar in the Batch panel.
- Clang-format file.

### Changed
- Domaine name is now www.fasttrack.sh.
- Parameter table in the Batch panel.

### Fixed
- Zoom and pan anchored on the mouse pointer.
- Automatic parameter file in the Batch panel.

## 4.5.0

### Added
- Auto load background image and settings file in the Tracking Inspector.
- Zoom in the Tracking Inspector and in the Interactive Tracking panels.
- Delete object in the Tracking Inspector.
- Added image counter in the Interactive Tracking.

### Fixed
- Fixed region of interest selection.


## 4.4.1

### Added
- Delete object in the Tracking Inspector.
- Background type automatic detection.

### Fixed
- Tracking crash if images are blank.
- Fixed multiple ui errors.
- Maximal distance and id assignment in Tracking.

### Changed
- Maximal distance in Tracking.
