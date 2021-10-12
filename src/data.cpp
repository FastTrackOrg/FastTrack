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
  if (!isEmpty) {
    save();
  }
  data.clear();
  dataCopy.clear();
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

  QVector<QString> replayTracking;

  QFile trackingFile(dir + QDir::separator() + "tracking.txt");
  if (trackingFile.exists() && trackingFile.open(QIODevice::ReadOnly)) {
    QTextStream input(&trackingFile);
    QString line;
    while (input.readLineInto(&line)) {
      replayTracking.append(line);
    }

    // Gets the header to construct the QMap data
    QStringList header = replayTracking[0].split('\t', Qt::SkipEmptyParts);
    replayTracking.removeFirst();

    for (auto &a : replayTracking) {
      QVector<object> tmpVector;
      QMap<int, QMap<QString, double>> objectMap;  // Map object id to data
      QMap<QString, double> objectData;            // Map data keys to data value

      QStringList dat = a.split('\t', Qt::SkipEmptyParts);
      if (dat.size() != 23) break;  // Checks for corrupted data
      int frameIndex = dat[21].toInt();
      if (frameIndex > maxFrameIndex) maxFrameIndex = frameIndex;
      int id = dat[22].toInt();
      if (id > maxId) maxId = id;
      dat.removeAt(22);

      for (int j = 0; j < dat.size(); j++) {
        objectData.insert(header[j], dat[j].toDouble());
      }

      tmpVector = data.value(frameIndex);
      object tmpObject;
      tmpObject.id = id;
      tmpObject.data = objectData;
      tmpVector.append(tmpObject);
      data.insert(frameIndex, tmpVector);
    }
    dataCopy = data;
    isEmpty = false;
    return true;
  }
  else {
    isEmpty = true;
    return false;
  }
}

/**
  * @brief Construct the data object from a tracking result file.
  * @param[in] dataPath Path to the tracking data file.
*/
Data::Data(const QString &dataPath) {
  isEmpty = true;
  setPath(dataPath);
}

/**
  * @brief Construct the data object from a tracking result file.
*/
Data::Data() {
  isEmpty = true;
}

/**
  * @brief Get the tracking data at the selected image number for all the objects.
  * @param[in] imageIndex The index of the image where to extracts the data.
  * @return The tracking data in a QVector that contains a structure with the Id of the object and data for this object. The data are stored in a QMap<dataName, value>. 
*/
QVector<object> Data::getData(int imageIndex) const {
  return data.value(imageIndex);
}

/**
  * @brief Get the tracking data at the selected image number for one selected object.
  * @param[in] imageIndex The index of the image where to extracts the data.
  * @param[in] id The id of the object.
  * @return The tracking data for for the selected object at the selected image. The data are stored in a QMap<dataName, value>. 
*/
QMap<QString, double> Data::getData(int imageIndex, int id) const {
  QVector<object> objects = data.value(imageIndex);
  for (auto &a : objects) {
    if (a.id == id) {
      return a.data;
      break;
    }
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
QMap<QString, QVector<double>> Data::getDataId(int id) const {
  QMap<QString, QVector<double>> idData;
  for (int i = 0; i < maxFrameIndex + 1; i++) {
    QMap<QString, double> tmp = getData(i, id);
    QMap<QString, double>::const_iterator j = tmp.constBegin();
    while (j != tmp.constEnd()) {
      QVector<double> tmpVal;
      if (idData.contains(j.key())) {
        tmpVal = idData.value(j.key());
      }
      tmpVal.append(j.value());
      idData.insert(j.key(), tmpVal);
      ++j;
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
  QVector<object> objects = data.value(imageIndex);
  QList<int> ids;
  for (auto &a : objects) {
    ids.append(a.id);
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
  QList<int> ids;
  for (int i = imageIndexFirst; i < imageIndexLast; i++) {
    QVector<object> objects = data.value(i);
    for (auto &a : objects) {
      ids.append(a.id);
    }
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
  int firstAppearance;
  for (int i = 0; i < maxFrameIndex + 1; i++) {
    QVector<object> objects = data.value(i);
    for (auto &a : objects) {
      if (a.id == objectId) {
        firstAppearance = i;
        return firstAppearance;
      }
    }
  }
  return 0;
}

/**
  * @brief In the tracking data, swaps two objects from a selected index to the end.
  * @arg[in] firstObject The first object id.
  * @arg[in] secondObject The second object id.
  * @arg[in] from Start index from which the data will be swapped.
*/
void Data::swapData(int firstObject, int secondObject, int from) {
  QMapIterator<int, QVector<object>> i(data);
  while (i.hasNext()) {
    i.next();
    if (i.key() >= from) {
      QVector<object> objects = i.value();

      for (int j = 0; j < objects.size(); j++) {
        int id = objects[j].id;
        int changed = -1;

        if (id == firstObject && j != changed) {
          objects[j].id = secondObject;
          changed = j;
        }
        else if (id == secondObject && j != changed) {
          objects[j].id = firstObject;
          changed = j;
        }
      }

      data.insert(i.key(), objects);
    }
  }
  save(false);
  dataCopy = data;
}

/**
  * @brief Delete the tracking data of one object from a selected index to the end.
  * @arg[in] objectId The object id.
  * @arg[in] from Start index from which the data will be swapped.
  * @arg[in] to End index from which the data will be swapped.
*/
void Data::deleteData(int objectId, int from, int to) {
  QMapIterator<int, QVector<object>> i(data);
  while (i.hasNext()) {
    i.next();
    if (i.key() >= from && i.key() <= to) {
      QVector<object> objects = i.value();

      for (int j = 0; j < objects.size(); j++) {
        if (objects[j].id == objectId) {
          objects.remove(j);
        }
      }
      data.insert(i.key(), objects);
    }
  }
  save(false);
}

/**
  * @brief Insert the tracking data for one object from a selected index to the end.
  * @arg[in] objectId The object id.
  * @arg[in] from Start index from which the data will be swapped.
  * @arg[in] to End index from which the data will be swapped.
*/
void Data::insertData(int objectId, int from, int to) {
  QMapIterator<int, QVector<object>> i(dataCopy);
  while (i.hasNext()) {
    i.next();
    if (i.key() >= from && i.key() <= to) {
      QVector<object> objects = i.value();

      for (int j = 0; j < objects.size(); j++) {
        if (objects[j].id == objectId) {
          object objectToInsert = dataCopy.value(i.key())[j];
          QVector<object> objectsToReplace = data.value(i.key());
          objectsToReplace.append(objectToInsert);
          data.insert(i.key(), objectsToReplace);
        }
      }
    }
  }
  save(false);
}
/**
  * @brief Save the data in the tracking result file.
  * @param[in] force Force the saving.
  * @param[in] eachActions Save every eachActions actions.
*/
void Data::save(bool force, int eachActions) {
  if (!force && actions < 10) {
    ++actions;
    return;
  }
  // Check if the class is running with an ui
  if (!QApplication::topLevelWidgets().isEmpty()) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
  }
  QFile file(dir + QDir::separator() + "tracking.txt");
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream out(&file);
    out << "xHead" << '\t' << "yHead" << '\t' << "tHead" << '\t' << "xTail" << '\t' << "yTail" << '\t' << "tTail" << '\t' << "xBody" << '\t' << "yBody" << '\t' << "tBody" << '\t' << "curvature" << '\t' << "areaBody" << '\t' << "perimeterBody" << '\t' << "headMajorAxisLength" << '\t' << "headMinorAxisLength" << '\t' << "headExcentricity" << '\t' << "tailMajorAxisLength" << '\t' << "tailMinorAxisLength" << '\t' << "tailExcentricity" << '\t' << "bodyMajorAxisLength" << '\t' << "bodyMinorAxisLength" << '\t' << "bodyExcentricity" << '\t' << "imageNumber" << '\t' << "id" << Qt::endl;
    QMapIterator<int, QVector<object>> i(data);
    while (i.hasNext()) {
      i.next();
      for (auto &a : i.value()) {
        out << a.data.value("xHead") << '\t' << a.data.value("yHead") << '\t' << a.data.value("tHead") << '\t' << a.data.value("xTail") << '\t' << a.data.value("yTail") << '\t' << a.data.value("tTail") << '\t' << a.data.value("xBody") << '\t' << a.data.value("yBody") << '\t' << a.data.value("tBody") << '\t' << a.data.value("curvature") << '\t' << a.data.value("areaBody") << '\t' << a.data.value("perimeterBody") << '\t' << a.data.value("headMajorAxisLength") << '\t' << a.data.value("headMinorAxisLength") << '\t' << a.data.value("headExcentricity") << '\t' << a.data.value("tailMajorAxisLength") << '\t' << a.data.value("tailMinorAxisLength") << '\t' << a.data.value("tailExcentricity") << '\t' << a.data.value("bodyMajorAxisLength") << '\t' << a.data.value("bodyMinorAxisLength") << '\t' << a.data.value("bodyExcentricity") << '\t' << a.data.value("imageNumber") << '\t' << a.id << Qt::endl;
      }
    }
    file.close();
  }
  else {
    QMessageBox msgBox;
    msgBox.setText("Error writting the tracking.txt file. Changes not saved.");
    msgBox.exec();
  }
  actions = 0;
  if (!QApplication::topLevelWidgets().isEmpty()) {
    QApplication::restoreOverrideCursor();
  }
}

Data::~Data() {
  if (!isEmpty && actions != 0) {
    save();
  }
};

SwapData::SwapData(int firstObject, int secondObject, int from, Data *data)
    : m_firstObject(firstObject), m_secondObject(secondObject), m_from(from), m_data(data) { setText("swap data"); }
void SwapData::redo() { m_data->swapData(m_firstObject, m_secondObject, m_from); }
void SwapData::undo() { m_data->swapData(m_secondObject, m_firstObject, m_from); }

DeleteData::DeleteData(int object, int from, int to, Data *data)
    : m_object(object), m_from(from), m_to(to), m_data(data) { setText("delete data"); }
void DeleteData::redo() { m_data->deleteData(m_object, m_from, m_to); }
void DeleteData::undo() { m_data->insertData(m_object, m_from, m_to); }
