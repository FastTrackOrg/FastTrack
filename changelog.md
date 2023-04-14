# FastTrack changelog

## 6.3.1

### Added
- Added fit to view at image loading.
- Menus keyboard shortcuts.

### Fixed
- Fixed cfg.toml formatting.

## 6.3.0

### Added
- Added drag and drop for video opening.

### Changed
- Changed display to a QGraphicsView for better zoom and pan.
- OpenCV upgrade to 4.7.0
- Qt upgrade to 6.4.2.

### Deprecated
- Remove openmp dependency

## 6.2.7

### Added
- Added donation support.
- Translation capability.

### Fixed
- Fixed deprecated API.
- String translation.
- Fixed qApp calls.

### Refactored
- Refactored UI.
- Refactored CI.
- Tracking class.

### Optimized
- QMap to QHash.

### Deprecated
- Removed Stat Analysis class.

## 6.2.6

### Added
- New version download button.
- Stat_analysis feature preview.

### Changed
- Update to Qt6.2.4 LTS.

### Removed
- Remove webengine requirement.

### Refactored
- Check for updates.

## 6.2.5

### Fixed
- Fixed memory leaks on Windows by disabling OpenCL

### Optimized
- Disabled OpenCL increases performance by 55% on Windows.

## 6.2.4

### Changed
- Optimized object's direction computation. Analysis is now ~15% faster.

### Added
- UI geometry and state saving.

## 6.2.3

### Added
- Added background status in interactive tracking.

### Fixed
- Fixed crop in interactive tracking.

## 6.2.2

### Added
- Enforced FFMPEG backend.
- Added tests for Windows and MacOS.
- Added clang clazy and tidy code checks.

### Fixed
- Fixed convergence criterion in AutoLevel.
- Fixed VideoReader behavior.
- Fixed several minor memory leaks in the UI.
- Fixed ctor parent argument for QObject.
- Fixed move and assignment move ctor to follow cppcoreguidelines.

### Optimized
- Optimized qstring-allocations.
- Optimized Data reading.
- Optimized image display.
- Optimized args-by-ref and args-by-value.

### Changed
- Update to Qt6.2.3 LTS.

## 6.2.1

### Added
- Added log file.

### Fixed
- Fixed tracking overlay slowdowns.

## 6.2.0

### Added
- Added FastTrack command line interface for Windows.

### Changed
- Changed compiler for Windows now using MinGW_64.

### Fixed
- Fixed tray behavior.
- Fixed closing behavior.
- Fixed start-up message behavior.
- Fixed stdout for the cli on Windows.

## 6.1.2

### Fixed
- Fixed error handling for corrupted movies.
- Fixed crash when browsing corrupted movies.
- Fixed compilation flags leading to performance improvement.
- Fixed AppImage.

### Changed
- Changed error triggering for corrupted movies. Background computation will stop and trigger a fatal error. 
- Thread-safe background computation.
- Update to Qt6.2.2 LTS and OpenCV 4.5.5.

## 6.1.1

### Fixed
- Fixed database connection names crossover (critical).

## 6.1.0

### Changed
- Changed the backend from text files to SQLite database.
- Refactored Data, Tracking, and Replay class to optimize memory consumption.

### Fixed
- Stoped the tracking immediately if a fatal error is encountered.
- Fixed AppImage for non-ubuntu-based Linux distribution.

## 6.0.1

### Changed
- Remove analytic.
- Update Qt and OpenCV for Windows.

### Fixed
- Fixed icon display in the tray.
- Fixed AppImage.

## 6.0.0

### Changed
- Updated to Qt6.
- Added Expert mode to simplify the UI.

### Fixed
- Fixed window state at startup.

## 5.3.5

### Added
- Added system tray.

### Fixed
- Fixed outdated URLs.

## 5.3.4

### Added
- Added TD2 dataset page.

### Fix
- Fixed theme.
- Fixed Linux standardized desktop meta info and binary.

## 5.3.3

### Fixed
- Fixed video sequence opening.
- Fixed documentation tab.

### Added
- Added FastTrack light version.

## 5.3.2

### Changed
- Performance improvement in tracking data correction.

### Fixed
- Fixed annotation.

### Added
- Added waiting for cursor at loading.
- Added loading of previous analysis.

## 5.3.1

### Fixed
- Fixed memory leaks in Replay.
- Cropped in interactive.

## 5.3.0

### Changed
- Refactored UI with major performance improvement on the display.
- Default style changed.
- Ui settings file architecture.

### Fixed
- Fixed normAngle cfg loading.

## 5.2.4

### Fixed
- ROI loading from cfg file.
- Fixed crashing when accessing images.
- Cleared error when input background image not matching video size.

## 5.2.3

### Fixed
- Fixed Replay panel objects' selection not working inside the Interactive panel.
- Fixed configuration loading in the Interactive panel.
- Updated Qt for Windows and Mac to the latest LTS.

### Added
- Added errors message and log.

### Changed
- Changed behavior when images are not readable, they are now skipped and index added to the error log and message.

## 5.2.2

### Fixed
- Fixed video decoding errors by upgrading OpenCv (> 4.5.0).

## 5.2.1

### Fixed
- Fixed normalization parameters convergence.

### Changed
- Changed parameter.param to cfg.toml that will break the loading of old parameter files.

### Added
- Added cfg.toml loading in the Interactive panel.

## 5.2.0

### Fixed
- Fixed crash when the video file contains illegible frames.
- Fixed background computation to take all the frames.

### Added
- Added auto level of the soft tracking parameters.

## 5.1.7

### Fixed
- Fixed crash when computing the background and moving the timeline.
- Fixed crash when moving the timeline to the last image in the Interactive panel.

## 5.1.6

### Fixed
- Fixed opening incorrect file crash in the Replay panel.

### Added
- Autoplay with the timeline.
- Privacy settings.
- Added version parameter in the cli.

### Removed
- Removed deprecated play button in the Replay panel.

## 5.1.5

### Fixed
- Fixed small UI-related crashes.
- Fixed ci libstdc AppImage.
- Fixed zoom.

### Added
- Added keyboard shortcuts timeline. 

### Optimized
- Speed-up image display in the interactive panel.

## 5.1.4

### Fixed
- Added FFmpeg dependency for Windows release

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
- Fixed error of conversion in the input angle normalization tracking parameter (breaking retro compatibility).

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
- Changed UI design in the Interactive panel.
- Performance optimization
- Added fasttrack-cli (not included in fasttrack-GUI release)

### Added
- Added online message display at start-up if necessary

### Fixed
- Ui error that crashes the program when opening new tracking analysis in the Replay panel.

## 4.9.0

### Added
- Several error messages if input/output can be done.
- Added tracking annotation, each image of a tracking analysis can be annotated.
- Added object information in the Tracking Inspector.
- Added the Tracking Manager to log each tracking analysis performed with FastTrack.
- Added video compatibility.
- Added 16 bits compatibility.

### Fixed
- Fixed some UI inconsistencies.
- Fixed the list of objects displayed in the Tracking Inspector.
- Fixed registration method no changing in the batch tracking.

### Removed
- Removed deprecated benchmarking feature.

## 4.8.3

### Fixed
- Fixed saving error in the Replay panel.
- Fixed global background not working in the Batch panel.
- Minor UI fixes.

### Added
- Better keyboard shortcut in the Replay Panel.
- Button to delete one object on one image in the Replay panel.
- Scale to see the length parameter in the Interactive tracking.
- Automatic refresh in the Replay panel when a parameter is changed.

## 4.8.2

### Changed
- Changed Replay panel UI design.
- Changed Interactive panel UI design.

## 4.8.1

### Changed
- Old tracking data can be opened in the Replay panel.
- Changed UI design.

## 4.8.0

### Fixed
- Id numbering issue.
- Crop does not work properly if the rectangle gets out of the image.
- Fixed crash if tracking data are corrupted in the Replay panel.

### Changed
- Changed the id display in the Replay Panel, allowing the user to select any Id in the bottom combo box.
- New tracking analysis does not overwrite the previous but renames the previous one as Tracking_Result_DateOfTheNewAnalysis.
- An error is triggered if the input image background is not openable.

### Added
- New registration method: ECC and feature-based.
- Undo/redo capabilities in the Replay panel.

## 4.7.0

### Fixed
- Batch tracking crash on Windows.
- Minor file opening errors.

### Added
- Multi-paths selection in the Batch panel.
- Added layout options in the Interactive panel.

### Changed
-  Batch panel UI.

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
- Autoload background image and settings file in the Tracking Inspector.
- Zoom in the Tracking Inspector and the Interactive Tracking panels.
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
- Fixed multiple UI errors.
- Maximal distance and id assignment in Tracking.

### Changed
- Maximal distance in Tracking.

