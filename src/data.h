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
  QMap<int, QVector<object>> dataCopy;
  QMap<int, QVector<object>> data; /*!< Tracking data stored in a QMap, the keys are the image index and the value a vector of data stored in a structure with a field containing the object id and a field containing the data stored in a QMap where the keys are the data name and the velue the data value. */

 public:
  Data() = default;
  explicit Data(const QString &dataPath);
  Data(const Data &T) = delete;
  Data &operator=(const Data &T) = delete;
  ~Data();

  bool setPath(const QString &dataPath);
  QVector<object> getData(int imageIndex) const;
  QMap<QString, double> getData(int imageIndex, int id) const;
  QMap<QString, QVector<double>> getDataId(int id) const;
  QList<int> getId(int imageIndex) const;
  QList<int> getId(int imageIndexFirst, int imageIndexLast) const;
  int getObjectInformation(int objectId) const;
  void swapData(int firstObject, int secondObject, int from);
  void deleteData(int objectId, int from, int to);
  void insertData(int objectId, int from, int to);
  void save();
  int maxId;
  int maxFrameIndex;
  bool isEmpty;
};

class SwapData : public QUndoCommand {
 public:
  SwapData(int firstObject, int secondObject, int from, Data *data);
  void undo() override;
  void redo() override;

 private:
  int m_firstObject;
  int m_secondObject;
  int m_from;
  Data *m_data;
};

class DeleteData : public QUndoCommand {
 public:
  DeleteData(int object, int from, int to, Data *data);
  void undo() override;
  void redo() override;

 private:
  int m_object;
  int m_from;
  int m_to;
  Data *m_data;
};
#endif
