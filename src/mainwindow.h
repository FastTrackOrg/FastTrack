/*
This file is part of Fast Track.

    FastTrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastTrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastTrack.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QByteArray>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QUrl>
#include <QVariant>
#include "batch.h"
#include "interactive.h"
#include "ui_mainwindow.h"
#include "updater.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  MainWindow(const MainWindow &T) = delete;
  MainWindow &operator=(const MainWindow &T) = delete;
  MainWindow &operator=(MainWindow &&T) = delete;
  MainWindow(MainWindow &&T) = delete;
  ~MainWindow();

 private:
  Ui::MainWindow *ui; /*!< ui file from Qt designer. */
  void closeEvent(QCloseEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void openInteractive();
  QMdiSubWindow *newInteractiveWindow();
  QMdiSubWindow *newBatchWindow();
  void showInteractiveDocks(Interactive *interactive);
  void saveWorkspaceState(Interactive *interactive);
  void loadParameterControls(const QHash<QString, QVariant> &state);
  void applyParameterControls();
  void showReplayDocks(Interactive *interactive);
  void loadReplayDisplayControls(Replay *replay);
  void applyReplayDisplayControls();
  void updateInformationTable(QTableWidget *table, const QList<QString> &values);
  Interactive *activeInteractive() const;
  void updateWorkspaceActions();
  void applyTheme(const QString &theme);
  void refreshContextMenus(QMdiSubWindow *subWindow);
  Updater *updater;
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *settingsMenu;
  QMenu *helpMenu;
  QMenu *mdiModeMenu;
  QMenu *mdiArrangeMenu;
  QAction *previewAction;
  QAction *trackAction;
  QDockWidget *imageOptionsDock;
  QDockWidget *trackingOptionsDock;
  QDockWidget *controlOptionsDock;
  QHash<QString, QWidget *> parameterControls;
  QDockWidget *replayDisplayDock;
  QDockWidget *annotationDock;
  QDockWidget *informationDock;
  QComboBox *replayEllipseBox;
  QComboBox *replayArrowBox;
  QCheckBox *replayTraceBox;
  QSpinBox *replayTraceLengthBox;
  QCheckBox *replayNumbersBox;
  QSpinBox *replaySizeBox;
  QSpinBox *replayFpsBox;
  QLineEdit *annotationFindLine;
  QTextEdit *annotationEdit;
  QTableWidget *informationTable1;
  QTableWidget *informationTable2;
  QHash<Interactive *, QHash<QString, QVariant>> workspaceStates;
  QByteArray savedDockState;
  bool shuttingDown = false;
  QSystemTrayIcon *trayIcon;
  QSettings *settingsFile;
};

#endif  // MAINWINDOW_H
