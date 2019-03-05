#include "data.h"


Data::Data(QString dataPath) {
  
  QVector<QString> replayTracking;
  dir = dataPath;

  if( QDir().exists(dir + QDir::separator() + "Tracking_Result") ) {

    QFile trackingFile( dir + QDir::separator() + "Tracking_Result" + QDir::separator() + "tracking.txt");
    
    if (trackingFile.open(QIODevice::ReadOnly)) {
      QTextStream input(&trackingFile);
      QString line;
      while (input.readLineInto(&line)) {
        replayTracking.append(line);
      }
    }

    // Gets the header to construct the QMap data
    QStringList header = replayTracking.at(0).split('\t', QString::SkipEmptyParts);
    replayTracking.removeFirst();

    for(auto &a: replayTracking) {
      
      QVector<object> tmpVector;
      QMap<int, QMap<QString, double> > objectMap; // Map object id to data
      QMap<QString, double> objectData; // Map data keys to data value

      QStringList dat = a.split('\t', QString::SkipEmptyParts);
      int frameIndex = dat.at(21).toInt();
      int id = dat.at(22).toInt();
      dat.removeAt(22);

      for(int j = 0; j < dat.size(); j++) {
        objectData.insert(header.at(j), dat.at(j).toDouble());
      }

      tmpVector = data.value(frameIndex);
      object tmpObject;
      tmpObject.id = id;
      tmpObject.data = objectData;
      tmpVector.append(tmpObject);
      data.insert(frameIndex, tmpVector);
    }
  }
}

QVector<object> Data::getData(int imageIndex) {
  return data.value(imageIndex);
}


QMap<QString, double> Data::getData(int imageIndex, int id) {
  QVector<object> objects = data.value(imageIndex);
  for(auto &a: objects) {
    if (a.id == id) {
      return a.data;
      break;
    }
  }
  QMap<QString, double> tmp;
  tmp.insert("NAN", -1);
  return tmp;
}



QList<int> Data::getId(int imageIndex) {

  QVector<object> objects = data.value(imageIndex);
  QList<int> ids;
  for(auto &a: objects) {
    ids.append(a.id);
  }
  return ids;
}

/**
  * @brief In the tracking data, swaps two objects from a selected index to the end.
  * @arg[in] firstObject The first object id.
  * @arg[in] secondObject The second object id.
  * @arg[in] from Start index from which the data will be swapped.
*/
void Data::swapData(int firstObject, int secondObject, int from) {
    
      QMapIterator<int, QVector<object> > i(data);
      while (i.hasNext()) {
        i.next();
        if(i.key() >= from) { 
          QVector<object> objects = i.value();
           
          for(int j = 0; j < objects.size(); j++) {
        
            int id = objects.at(j).id;
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
}


void Data::save() {

  QFile file(dir + QDir::separator() + "Tracking_Result" + QDir::separator() + "tracking.txt");
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream out(&file);
    out << "xHead" << '\t' << "yHead" << '\t' << "tHead" << '\t'  << "xTail" << '\t' << "yTail" << '\t' << "tTail"   << '\t'  << "xBody" << '\t' << "yBody" << '\t' << "tBody"   << '\t'  << "curvature"  << '\t' << "areaBody" << '\t' << "perimeterBody" << '\t' << "headMajorAxisLength" << '\t' << "headMinorAxisLength" << '\t' << "headExcentricity" << '\t' << "tailMajorAxisLength" << '\t' << "tailMinorAxisLength" << '\t' << "tailExcentricity" << '\t'<< "bodyMajorAxisLength" << '\t' << "bodyMinorAxisLength" << '\t' << "bodyExcentricity" << '\t' << "imageNumber" << '\t' << "id" << endl;
    QMapIterator<int, QVector<object> > i(data);
    while (i.hasNext()) {
      i.next();
      for(auto& a: i.value() ) {
        out << a.data.value("xHead") << '\t' << a.data.value("yHead") << '\t' << a.data.value("tHead") << '\t'  << a.data.value("xTail") << '\t' << a.data.value("yTail") << '\t' << a.data.value("tTail")   << '\t'  << a.data.value("xBody") << '\t' << a.data.value("yBody") << '\t' << a.data.value("tBody")   << '\t'  << a.data.value("curvature")  << '\t' << a.data.value("areaBody") << '\t' << a.data.value("perimeterBody") << '\t' << a.data.value("headMajorAxisLength") << '\t' << a.data.value("headMinorAxisLength") << '\t' << a.data.value("headExcentricity") << '\t' << a.data.value("tailMajorAxisLength") << '\t' << a.data.value("tailMinorAxisLength") << '\t' << a.data.value("tailExcentricity") << '\t'<< a.data.value("bodyMajorAxisLength") << '\t' << a.data.value("bodyMinorAxisLength") << '\t' << a.data.value("bodyExcentricity") << '\t' << a.data.value("imageNumber") << '\t' << a.id << endl;
      }
    }
  file.close();
  }
}
