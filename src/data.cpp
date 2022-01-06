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

#include "data.h"

/**
 * @class Data
 *
 * @brief This class allows to load tracking data produced by the Tracking class.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 5.0 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */

/**
 * @brief Clear data.
 */
void Data::clear() {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  if (!isEmpty) {
    save();
    QSqlQuery query(data);
    query.exec("DROP TABLE deleted");
  }
  data.close();
  maxId = 0;
  maxFrameIndex = 0;
  actions = 0;
  dir.clear();
  isEmpty = true;
}

/**
 * @brief Set the path to a tracking data file.
 * @param[in] dataPath Path to the tracking data file.
 */
bool Data::setPath(const QString &dataPath) {
  clear();
  dir = dataPath;

  QSqlDatabase data = QSqlDatabase::database("data", false);
  data.setDatabaseName(dataPath + "/tracking.db");
  QSqlQuery query(data);
  if (data.open()) {
    query.exec("CREATE TABLE deleted ( xHead REAL, yHead REAL, tHead REAL, xTail REAL, yTail REAL, tTail REAL, xBody REAL, yBody REAL, tBody REAL, curvature REAL, areaBody REAL, perimeterBody REAL, headMajorAxisLength REAL, headMinorAxisLength REAL, headExcentricity REAL, tailMajorAxisLength REAL, tailMinorAxisLength REAL, tailExcentricity REAL, bodyMajorAxisLength REAL, bodyMinorAxisLength REAL, bodyExcentricity REAL, imageNumber INTEGER, id INTEGER)");
    query.exec("SELECT MAX(imageNumber), MAX(id) FROM tracking");
    if (query.first()) {
      maxFrameIndex = query.value(0).toInt();
      maxId = query.value(1).toInt();
      isEmpty = false;
      return true;
    }
  }
  isEmpty = true;
  return false;
}

/**
 * @brief Construct the data object from a tracking result file.
 * @param[in] dataPath Path to the tracking data file.
 */
Data::Data(const QString &dataPath) : Data() {
  setPath(dataPath);
}

/**
 * @brief Construct the data object from a tracking result file.
 */
Data::Data() {
  isEmpty = true;
  QSqlDatabase data = QSqlDatabase::addDatabase("QSQLITE", "data");
}

/**
 * @brief Get the tracking data at the selected image number for one selected object.
 * @param[in] imageIndex The index of the image where to extracts the data.
 * @param[in] id The id of the object.
 * @return The tracking data for for the selected object at the selected image. The data are stored in a QMap<dataName, value>.
 */
QMap<QString, double> Data::getData(int imageIndex, int id) const {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("SELECT * FROM tracking WHERE imageNumber = ? AND id = ?");
  query.addBindValue(imageIndex);
  query.addBindValue(id);
  query.exec();
  QSqlRecord rec = query.record();
  if (query.first()) {
    QMap<QString, double> tmp;
    for (int i = 0; i < rec.count(); i++) {
      tmp.insert(rec.fieldName(i), query.value(i).toDouble());
    }
    return tmp;
  }
  QMap<QString, double> tmp;
  tmp.insert("NAN", -1);
  return tmp;
}

/**
 * @brief Get the tracking data for the selected id.
 * @param[in] id The id of the object.
 * @return The tracking data for for the selected object in a map with the key and a vector with the value for all the images.
 */
QMap<QString, QList<double>> Data::getDataId(int id) const {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("SELECT xHead, yHead, tHead, xTail, yTail, tTail, xBody, yBody, tBody, curvature, areaBody, perimeterBody, headMajorAxisLength, headMinorAxisLength, headExcentricity, tailMajorAxisLength, tailMinorAxisLength, tailExcentricity, bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity, imageNumber, id FROM tracking WHERE id = ?");
  query.addBindValue(id);
  query.exec();
  QSqlRecord rec = query.record();
  QMap<QString, QList<double>> idData;
  for (int i = 0; i < rec.count(); i++) {
    idData.insert(rec.fieldName(i), {});
  }
  while (query.next()) {
    for (int i = 0; i < rec.count(); i++) {
      idData[rec.fieldName(i)].append(query.value(i).toDouble());
    }
  }
  return idData;
}

/**
 * @brief Get the ids of all the objects in the frame.
 * @arg[in] imageIndex Index of the frame.
 * @return List of indexes.
 */
QList<int> Data::getId(int imageIndex) const {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("SELECT id FROM tracking where imageNumber = ?");
  query.addBindValue(imageIndex);
  query.exec();
  QList<int> ids;
  while (query.next()) {
    ids.append(query.value(0).toInt());
  }
  return ids;
}

/**
 * @brief Get the ids of all the objects in several frames.
 * @arg[in] imageIndexFirst Index of the first frame.
 * @arg[in] imageIndexLast Index of the last frame.
 * @return List of indexes.
 */
QList<int> Data::getId(int imageIndexFirst, int imageIndexLast) const {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("SELECT id FROM tracking where imageNumber >= ? AND imageNumber <= ?");
  query.addBindValue(imageIndexFirst);
  query.addBindValue(imageIndexLast);
  query.exec();
  QList<int> ids;
  while (query.next()) {
    ids.append(query.value(0).toInt());
  }
  std::sort(ids.begin(), ids.end());
  ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
  return ids;
}

/**
 * @brief Get the object's information.
 * @arg[in] objectId Id of the object.
 * @return First appearance image index.
 */
int Data::getObjectInformation(int objectId) const {
  int firstAppearance = 0;
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("SELECT MIN(imageNumber) FROM tracking where id = ?");
  query.addBindValue(objectId);
  query.exec();
  if (query.first()) {
    firstAppearance = query.value(0).toInt();
  }
  return firstAppearance;
}

/**
 * @brief In the tracking data, swaps two objects from a selected index to the end.
 * @arg[in] firstObject The first object id.
 * @arg[in] secondObject The second object id.
 * @arg[in] from Start index from which the data will be swapped.
 */
void Data::swapData(int firstObject, int secondObject, int from) {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("UPDATE tracking SET id = (CASE WHEN id = :first THEN :second ELSE :first END) WHERE id IN (:first, :second) AND imageNumber >= :from");
  query.bindValue(":first", firstObject);
  query.bindValue(":second", secondObject);
  query.bindValue(":from", from);
  query.exec();
  save(false);
}

/**
 * @brief Delete the tracking data of one object from a selected index to the end.
 * @arg[in] objectId The object id.
 * @arg[in] from Start index from which the data will be deleted.
 * @arg[in] to End index from which the data will be deleted.
 */
void Data::deleteData(int objectId, int from, int to) {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("INSERT INTO deleted SELECT xHead, yHead, tHead, xTail, yTail, tTail, xBody, yBody, tBody, curvature, areaBody, perimeterBody, headMajorAxisLength, headMinorAxisLength, headExcentricity, tailMajorAxisLength, tailMinorAxisLength, tailExcentricity, bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity, imageNumber, id FROM tracking WHERE id = ? AND imageNumber >= ? AND imageNumber <= ?");
  query.addBindValue(objectId);
  query.addBindValue(from);
  query.addBindValue(to);
  query.exec();
  query.prepare("DELETE FROM tracking WHERE id = ? AND imageNumber >= ? AND imageNumber <= ?");
  query.addBindValue(objectId);
  query.addBindValue(from);
  query.addBindValue(to);
  query.exec();
  save(false);
}

/**
 * @brief Insert the tracking data for one object from a selected index to the end.
 * @arg[in] objectId The object id.
 * @arg[in] from Start index from which the data will be swapped.
 * @arg[in] to End index from which the data will be swapped.
 */
void Data::insertData(int objectId, int from, int to) {
  QSqlDatabase data = QSqlDatabase::database("data", false);
  QSqlQuery query(data);
  query.prepare("INSERT INTO tracking SELECT xHead, yHead, tHead, xTail, yTail, tTail, xBody, yBody, tBody, curvature, areaBody, perimeterBody, headMajorAxisLength, headMinorAxisLength, headExcentricity, tailMajorAxisLength, tailMinorAxisLength, tailExcentricity, bodyMajorAxisLength, bodyMinorAxisLength, bodyExcentricity, imageNumber, id FROM deleted WHERE id = ? AND imageNumber >= ? AND imageNumber <= ?");
  query.addBindValue(objectId);
  query.addBindValue(from);
  query.addBindValue(to);
  query.exec();
  query.prepare("DELETE FROM deleted WHERE id = ? AND imageNumber >= ? AND imageNumber <= ?");
  query.addBindValue(objectId);
  query.addBindValue(from);
  query.addBindValue(to);
  query.exec();
  save(false);
}

/**
 * @brief Save the data in the tracking result file.
 * @param[in] force Force the saving.
 * @param[in] eachActions Save every eachActions actions.
 */
void Data::save(bool force, int eachActions) {
  if (!force && actions < eachActions) {
    ++actions;
    return;
  }
  // Check if the class is running with an ui
  if (!QApplication::topLevelWidgets().isEmpty()) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
  }
  QString file = dir + QDir::separator();
  QSqlDatabase data = QSqlDatabase::database("data", false);
  Tracking::exportTrackingResult(file, data);
  actions = 0;
  if (!QApplication::topLevelWidgets().isEmpty()) {
    QApplication::restoreOverrideCursor();
  }
}

Data::~Data() {
  clear();
  QSqlDatabase::removeDatabase("data");
};

SwapData::SwapData(int firstObject, int secondObject, int from, Data *data)
    : m_firstObject(firstObject), m_secondObject(secondObject), m_from(from), m_data(data) { setText("swap data"); }
void SwapData::redo() { m_data->swapData(m_firstObject, m_secondObject, m_from); }
void SwapData::undo() { m_data->swapData(m_secondObject, m_firstObject, m_from); }

DeleteData::DeleteData(int object, int from, int to, Data *data)
    : m_object(object), m_from(from), m_to(to), m_data(data) { setText("delete data"); }
void DeleteData::redo() { m_data->deleteData(m_object, m_from, m_to); }
void DeleteData::undo() { m_data->insertData(m_object, m_from, m_to); }
