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

#include "annotation.h"

/**
 * @class Annotation
 *
 * @brief This class allows to load tracking annotation file.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 5 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */

/**
 * @brief Clear the object.
 */
void Annotation::clear() {
  if (!annotationFile->fileName().isEmpty()) {
    writeToFile();
    annotationFile->close();
    annotationFile->setFileName(QLatin1String(""));
  }
  findIndexes.clear();
  findIndex = -1;
  annotations->clear();
  isActive = false;
}

/**
 * @brief Set the path for the annotation.
 * @param[in] filePath Path to the tracking folder.
 */
bool Annotation::setPath(const QString &filePath) {
  clear();
  annotationFile->setFileName(filePath + "annotation.txt");
  if (!annotationFile->open(QIODevice::ReadOnly)) {
    return false;
  }

  QTextStream in(annotationFile);
  QString data = in.readAll();
  QStringList annotationEntries = data.split(QStringLiteral("\n\t\n"));

  for (auto const &a : annotationEntries) {
    QStringList entry = a.split(QStringLiteral("\t\n\t"));
    if (entry.length() == 2) {
      annotations->insert(entry[0].toInt(), entry[1]);
    }
  }
  annotationFile->close();
  isActive = true;
  return true;
}

/**
 * @brief Constructs the annotation object from a file path.
 * @param[in] filePath Path to the tracking folder.
 */
Annotation::Annotation(const QString &filePath) : Annotation() {
  setPath(filePath);
}

/**
 * @brief Constructs the annotation object from a file path.
 */
Annotation::Annotation(QWidget *parent) : annotationFile{new QFile()}, annotations{new QMap<int, QString>()}, findIndex{-1}, isActive{false} {
}

/**
 * @brief Writes all the annotation to a file.
 */
void Annotation::writeToFile() {
  if (!annotationFile->open(QIODevice::WriteOnly)) {
    qInfo() << "Can't open file";
  }

  QTextStream out(annotationFile);
  QMapIterator<int, QString> i(*annotations);
  while (i.hasNext()) {
    i.next();
    out << i.key() << "\t\n\t" << i.value() << "\n\t\n";
  }
  annotationFile->close();
}

/**
 * @brief Adds an annotation to the annotation QMap.
 * @param[in] index Image index.
 * @param[in] text Annotation text.
 */
void Annotation::write(int index, const QString &text) {
  if (isActive) {
    annotations->insert(index, text);
    writeToFile();
  }
}

/**
 * @brief Reads an annotation from the annotation QMap.
 * @param[in] index Image index.
 */
void Annotation::read(int index) {
  if (isActive) {
    QString text = annotations->value(index);
    emit annotationText(text);
  }
}

/**
 * @brief Finds the index of all the annotation with expression inside their text.
 * @param[in] expression Expression to find, case sensitive.
 */
void Annotation::find(const QString &expression) {
  findIndexes.clear();
  QMapIterator<int, QString> i(*annotations);
  while (i.hasNext()) {
    i.next();
    if (i.value().contains(expression)) {
      findIndexes.append(i.key());
    }
  }
  findIndex = -1;
}

/**
 * @brief Returns the next element of the findIndexes list of annotations that contains the expression to find.
 */
int Annotation::next() {
  ++findIndex;
  if (findIndex >= findIndexes.length()) {
    findIndex = 0;
  }
  if (!findIndexes.isEmpty()) {
    return findIndexes[findIndex];
  }
  else {
    return 0;
  }
}

/**
 * @brief Returns the previous element of the findIndexes list of annotations that contains the expression to find.
 */
int Annotation::prev() {
  --findIndex;
  if (findIndex < 0) {
    findIndex = findIndexes.length() - 1;
  }
  if (!findIndexes.isEmpty()) {
    return findIndexes[findIndex];
  }
  else {
    return 0;
  }
}

Annotation::~Annotation() {
  if (isActive) {
    writeToFile();
  }
  delete annotationFile;
  delete annotations;
}
