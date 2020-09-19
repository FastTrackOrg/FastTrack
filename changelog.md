# FastTrack changelog

## 5.1.4

### Fixed
- Added ffmpeg dependency for Windows release

## 5.1.3

### Fixed
- Fixed background registration.
- Fixed annotation.
- Fixed tracking inspector crash in the Interactive panel.

### Added
- Added configuration file loading in the cli.

### Optimized
- Optimized video opening for compressed format.

## 5.1.2

### Fixed
- Fixed the image sequence opening.

## 5.1.1

### Fixed
- Fixed the timeline resizing.
- Fixed the timeline memory overload.

### Added
- Added timeline zoom.

## 5.1.0

### Fixed
- Fixed the zoom.
- Fixed the curvature computation.

### Added
- Added timeline replacing the slider.
- Added native video reading.
- Added area and perimeter in the cost function.

## 5.0.0

### Fixed
- Fixed error of conversion in the inputed angle normalization tracking parameter (breaking retrocompatibility).

### Removed
- Removed deprecated weight tracking parameter (breaking retro-compatibility).

## 4.9.2

### Added
- Added integrated help.
- Added theme selector.

### Fixed
- Fixed list of objects update in the Replay panel.

## 4.9.1

### Changed
- Changed ui design in the Interactive panel.
- Performance optimization
- Added fasttrack-cli (not included in fasttrack-gui release)

### Added
- Added online message display at start-up if necessary

### Fixed
- Ui error that crash the program when opening new tracking analysis in the Replay panel.

## 4.9.0

### Added
- Several error message if input/output can be done.
- Added tracking annotation, each image of a tracking analysis can be annoted.
- Added object information in the Tracking Inspector.
- Added the Tracking Manager to log each tracking analysis performed with FastTrack.
- Added video compatibility.
- Added 16 bits compatibility.

### Fixed
- Fixed some ui inconsistencies.
- Fixed the list of object displayed in the Tracking Inspector.
- Fixed registration method no changing in the batch tracking.

### Removed
- Removed deprecated benchmarking feature.

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
