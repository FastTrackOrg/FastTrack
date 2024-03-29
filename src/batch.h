#ifndef BATCH_H
#define BATCH_H

#include <stdlib.h>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QKeySequence>
#include <QLCDNumber>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSharedPointer>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>
#include <QTreeView>
#include <QWidget>
#include <ctime>
#include <fstream>
#include <opencv2/core/ocl.hpp>
#include <string>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "tracking.h"

namespace Ui {
class Batch;
}

class Batch : public QWidget {
  Q_OBJECT

 public:
  explicit Batch(QWidget *parent = nullptr);
  Batch(const Batch &T) = delete;
  Batch &operator=(const Batch &T) = delete;
  Batch &operator=(Batch &&T) = delete;
  Batch(Batch &&T) = delete;

  ~Batch();

 private:
  Ui::Batch *ui;                         /*!< ui file from Qt designer. */
  QHash<QString, QString> parameterList; /*!< All the parameters necessary for the tracking analysis. */

  QThread *thread;    /*!< Thread where lives the Tracking object. */
  Tracking *tracking; /*!< Objects that track images sequence. */

  QShortcut *wShortcut; /*!< Keyboard shortcut to next occlusion. */
  QShortcut *qShortcut; /*!< Keyboard shortcut to previous frame. */
  QShortcut *aShortcut; /*!< Keyboard shortcut to previous frame. */
  QShortcut *dShortcut; /*!< Keyboard shortcut to next frame. */

  struct process
  {
    QString path;
    QString backgroundPath;
    QHash<QString, QString> trackingParameters;
  };
  QList<process> processList;

  void loadSettings();
  void saveSettings();
  QSettings *settingsFile; /*!< Saves parameters in a settings.ini file.*/
  QString memoryDir;       /*!< Saves the path of the last opened folder.*/
  bool isEditable;
  int currentPathCount;

  //    Replay *replayPanel;

 public slots:
  void updateParameters();
  void startTracking();
  void openPathFolder();
  void openPathBackground(int);
  void addPath(const QString &, const QString &, const QString &);
  void removePath();
  void removePath(int index);
  void updateParameterTable();
  bool loadParameterFile(const QString &path);
  void openParameterFile(int);

  void errors(int code);
 signals:
  /**
   * @brief Emitted when a parameter is changed.
   * @param parameterList All parameters necessary to the tracking analysis.
   */
  void newParameterList(const QHash<QString, QString> &parameterList);

  /**
   * @brief Emitted when a tracking analysis is finished.
   */
  void next();
  void log(QHash<QString, QString> log);
  void status(QString messsage);
};

#endif  // BATCH_H
