#ifndef DATA_H
#define DATA_H

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QMap>
#include <QMapIterator>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <QUndoCommand>
#include <QWidget>
#include "tracking.h"

class Data {
 private:
  QString dir; /*!< Path to the tracking result file. */
  int actions;

 public:
  Data();
  explicit Data(const QString &dataPath);
  Data(const Data &T) = delete;
  Data &operator=(const Data &T) = delete;
  ~Data();

  const QString connectionName;
  bool setPath(const QString &dataPath);
  QHash<QString, double> getData(int imageIndex, int id) const;
  QList<QHash<QString, double>> getData(int imageIndex) const;
  QList<QHash<QString, double>> getData(int imageIndexStart, int imageIndexStop, int id) const;
  QHash<QString, QList<double>> getDataId(int id) const;
  QList<int> getId(int imageIndex) const;
  QList<int> getId(int imageIndexFirst, int imageIndexLast) const;
  int getObjectInformation(int objectId) const;
  void swapData(int firstObject, int secondObject, int from);
  void deleteData(int objectId, int from, int to);
  void insertData(int objectId, int from, int to);
  void save(bool force = true, int eachActions = 30);
  void clear();
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
