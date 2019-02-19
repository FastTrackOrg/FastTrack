#ifndef BATCH_H
#define BATCH_H

#include <QWidget>
#include <vector>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>
#include <QCheckBox>
#include <QSlider>
#include <QLCDNumber>
#include <QKeySequence>
#include <QComboBox>
#include <QThread>
#include <QSpinBox>
#include <stdlib.h>
#include <QLayout>
#include <QWidget>
#include <QMap>
#include <QDir>
#include <QString>
#include <QShortcut>
#include <QSettings>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QTableWidgetItem>
#include "tracking.h"
#include "ui_batch.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <opencv2/core/ocl.hpp>
#include <ctime>

namespace Ui {
class Batch;
}

class Batch : public QWidget
{
    Q_OBJECT

public:
    explicit Batch(QWidget *parent = nullptr);
    ~Batch();
    
private:
    Ui::Batch *ui;    /*!< ui file from Qt designer. */
    QMap<QString, QString> parameterList;   /*!< All the parameters necessary for the tracking analysis. */
    
    QThread* thread;   /*!< Thread where lives the Tracking object. */
    Tracking* tracking;   /*!< Objects that track images sequence. */

    QShortcut *wShortcut;   /*!< Keyboard shortcut to next occlusion. */
    QShortcut *qShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *aShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *dShortcut;   /*!< Keyboard shortcut to next frame. */

    struct process {
      QString path;
      QString backgroundPath;
      QMap<QString, QString> trackingParameters;
    };
    QVector<process> processList;




    void loadSettings();
    void saveSettings();
    QSettings *settingsFile;   /*!< Saves parameters in a settings.ini file.*/
    QString memoryDir;   /*!< Saves the path of the last opened folder.*/
    
//    Replay *replayPanel;


public slots:
    void updateParameterList(QTableWidgetItem* item);
    void startTracking();
    void openPathFolder();
    void openPathBackground();
    void addPath();
    void removePath();
    void updateParameterTable();
    void loadParameterFile(QString path);
    void openParameterFile();

    void errors(int code);
signals:
  /**
  * @brief Emitted when a parameter is changed.
  * @param parameterList All parameters necessary to the tracking analysis.
  */
    void newParameterList(const QMap<QString, QString> &parameterList);
  
  /**
  * @brief Emitted when a tracking analysis is finished.
  */
    void next();


};

#endif // BATCH_H
