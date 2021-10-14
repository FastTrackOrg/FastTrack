#ifndef TIMELINE_H
#define TIMELINE_H

#include <QBrush>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPen>
#include <QScrollBar>
#include <QShortcut>
#include <QString>
#include <QTimer>
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
  int currentValue();
  void togglePlay();
  int isAutoplay;

 private:
  Ui::Timeline *ui;

  int m_imageNumber;
  int m_imageMin;
  int m_width;
  int m_offset;
  int m_currentIndex;
  int m_currentIndexLeft;
  int m_scale;
  QTimer *timer;

  QGraphicsScene *timelineScene;
  QGraphicsLineItem *cursor;
  QGraphicsLineItem *cursorLeft;
  QGraphicsSimpleTextItem *indexNumber;

  QList<int> markers;

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
