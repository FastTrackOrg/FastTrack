#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QHashIterator>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QUndoCommand>
#include <QWidget>

class Annotation : public QWidget {
  Q_OBJECT

 private:
  QFile *annotationFile;
  QHash<int, QString> *annotations;
  void writeToFile();
  QList<int> findIndexes;
  qsizetype findIndex;

 public slots:
  void clear();
  void write(int index, const QString &text);
  void read(int index);
  void find(const QString &expression);
  int next();
  int prev();

 signals:
  /**
   * @brief Emitted when a new annotation is read.
   * @param text Text of the requested annotation.
   */
  void annotationText(const QString &text);

 public:
  explicit Annotation(QWidget *parent = nullptr);
  Annotation(const QString &annotationFile);
  Annotation(const Annotation &T) = delete;
  Annotation &operator=(const Annotation &T) = delete;
  Annotation &operator=(Annotation &&T) = delete;
  Annotation(Annotation &&T) = delete;
  ~Annotation();
  bool setPath(const QString &annotationFile);
  bool isActive;
};

#endif
