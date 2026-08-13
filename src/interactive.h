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

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QAction>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaType>
#include <QPainter>
#include <QProgressBar>
#include <QRadioButton>
#include <QResizeEvent>
#include <QScopedValueRollback>
#include <QScrollBar>
#include <QSharedPointer>
#include <QSpinBox>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QVariant>
#include <QWidget>
#include <QtConcurrent/QtConcurrentRun>
#include "autolevel.h"
#include "data.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "replay.h"
#include "tracking.h"
#include "videoreader.h"

using namespace std;
using namespace cv;

namespace Ui {
class Interactive;
}

class Interactive : public QMainWindow {
  Q_OBJECT

 public:
  explicit Interactive(QWidget *parent = nullptr);
  Interactive(const Interactive &T) = delete;
  Interactive &operator=(const Interactive &T) = delete;
  Interactive &operator=(Interactive &&T) = delete;
  Interactive(Interactive &&T) = delete;
  ~Interactive();
  void openFolder(QString path = QString());
  QHash<QString, QVariant> parameterState() const;
  void setParameterState(const QHash<QString, QVariant> &state);
  Replay *replayWidget() const;
  void preview();
  void startTracking();
  void computeWorkspaceBackground();
  void selectWorkspaceBackground();
  void cropWorkspace();
  void resetWorkspaceCrop();
  void levelWorkspaceParameters();
  bool canPreview() const;
  bool canTrack() const;
  void setReplayVisible(bool visible);
  bool isReplayVisible() const;
  bool isReplayActive() const;
  bool isTimelineEnabled() const;
  int frameCount() const;
  QList<QString> informationValues() const;
  QString trackingStatusText() const;

 private slots:
  void display(int index, int scale = 0);
  void display(const QImage &image);
  void display(const cv::UMat &image, QImage::Format format = QImage::Format_RGB888);

  void getParameters();

  void previewTracking();
  void track();

  void computeBackground();
  void selectBackground();

  bool eventFilter(QObject *target, QEvent *event) override;
  void crop();
  void reset();

  void loadParameters(const QString &path);
  void level();
  void setTrackingAvailable(bool available);
  void notifyParameterStateChanged();
  void updateProgressEstimate(int value);
  int parameterInt(const QString &name) const;
  double parameterDouble(const QString &name) const;
  void setParameter(const QString &name, const QVariant &value);

 private:
  Ui::Interactive *ui;
  struct RuntimeControls
  {
    QProgressBar *progressBar;
  } *controls = nullptr;
  QHash<QString, QVariant> parameterValues;
  QList<QString> informationValuesState{QString(), QStringLiteral("0"), QStringLiteral("0"), QStringLiteral("0"), QStringLiteral("0")};
  QString trackingStatusState;
  QLabel *counterLabel;
  bool replayVisible = false;
  QString memoryDir;                  /*!< Saves the path to the last opened folder in dialog. */
  QSize originalImageSize;            /*!< Size of the original image. */
  QSize cropedImageSize;              /*!< Size of the croped image. */
  QHash<QString, QString> parameters; /*!< Tracking parameters. */
  QString path;                       /*!< */
  QString backgroundPath;             /*!< Path to the background image. */
  QString dir;                        /*!< Path to the folder where the image sequence to display is stored. */
  Tracking *tracking;                 /*!< Tracking object. */
  UMat background;                    /*!< Background image. */
  bool isBackground;                  /*!< Is the background computed. */
  QPair<QPointF, QPointF> clicks;
  Rect roi;
  QPixmap resizedPix;
  vector<Point3i> colorMap;
  VideoReader *video;
  Replay *replay;
  bool videoStatus;
  bool displayInProgress = false;
  bool contourWarningOpen = false;
  int largeContourDisplayDecision = -1;
  bool trackingAvailable = false;
  bool timelineEnabled = true;
  bool applyingParameterState = false;
  QElapsedTimer trackingElapsed;

 signals:
  void message(QString message);
  void log(QHash<QString, QString> log);
  void status(QString messsage);
  void replayVisibleChanged(bool visible);
  void activeViewChanged(bool replayActive);
  void trackingAvailabilityChanged();
  void timelineEnabledChanged(bool enabled);
  void parameterStateChanged(const QHash<QString, QVariant> &state);
  void inputOpened(const QString &path);
};
#endif  // INTERACTIVE_H
