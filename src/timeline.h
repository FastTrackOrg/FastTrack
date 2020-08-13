#ifndef TIMELINE_H
#define TIMELINE_H

#include <QBrush>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QPen>
#include <QScrollBar>
#include <QString>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Timeline;
}
QT_END_NAMESPACE

class Timeline : public QWidget {
  Q_OBJECT

 public:
  Timeline(QWidget *parent = nullptr);
  ~Timeline();
  void setValue(const int index);
  void setCursorValue(const int index);
  void setMaximum(const int max);
  void setMinimum(const int min);
  int value();

 private:
  Ui::Timeline *ui;

  int m_imageNumber;
  int m_imageMin;
  int m_width;
  int m_offset;
  int m_currentIndex;
  int m_currentIndexLeft;
  int m_scale;

  QGraphicsScene *timelineScene;
  QGraphicsLineItem *cursor;
  QGraphicsLineItem *cursorLeft;
  QGraphicsSimpleTextItem *indexNumber;

  QVector<int> markers;

  void setLayout(const int width, const int imageNumber);
  void resizeEvent(QResizeEvent *event);
  bool eventFilter(QObject *target, QEvent *event);
  void drawMarker(const int index);
  void clearMarker(const int index);
  void update(const int index);

 signals:
  void valueChanged(int value);
};
#endif  // TIMELINE_H
