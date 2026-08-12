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
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QIcon>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QSettings>
#include <QSysInfo>
#include <QSystemTrayIcon>
#include <QUrl>
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
  void applyTheme(const QString &theme);
  void refreshContextMenus(QMdiSubWindow *subWindow);
  Updater *updater;
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *settingsMenu;
  QMenu *helpMenu;
  QMenu *mdiModeMenu;
  QMenu *mdiArrangeMenu;
  QSystemTrayIcon *trayIcon;
  QSettings *settingsFile;
};

#endif  // MAINWINDOW_H
