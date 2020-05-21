#ifndef DATA_H
#define DATA_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QMapIterator>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QUndoCommand>
#include <QWidget>

struct object
{
  int id;
  QMap<QString, double> data;
};

class Data {
 private:
  QString dir; /*!< Path to the tracking result file. */

 public:
  explicit Data(QString dataPath);
  ~Data();

  QMap<int, QVector<object>> data; /*!< Tracking data stored in a QMap, the keys are the image index and the value a vector of data stored in a structure with a field containing the object id and a field containing the data stored in a QMap where the keys are the data name and the velue the data value. */
  int maxId;
  int maxFrameIndex;

  QVector<object> getData(int imageIndex);
  QMap<QString, double> getData(int imageIndex, int id);
  QList<int> getId(int imageIndex);
  QList<int> getId(int imageIndexFirst, int imageIndexLast);
  int getObjectInformation(int objectId);
  void swapData(int firstObject, int secondObject, int from);
  void deleteData(int objectId, int from, int to);
  void insertData(int objectId, int from, int to);
  void save();

  QMap<int, QVector<object>> dataCopy;
};

class SwapData : public QUndoCommand {
 public:
  SwapData(int firstObject, int secondObject, int from, Data *data);
  void undo() override;
  void redo() override;

 private:
  Data *m_data;
  int m_firstObject;
  int m_secondObject;
  int m_from;
};

class DeleteData : public QUndoCommand {
 public:
  DeleteData(int object, int from, int to, Data *data);
  void undo() override;
  void redo() override;

 private:
  Data *m_data;
  int m_object;
  int m_from;
  int m_to;
};
#endif
