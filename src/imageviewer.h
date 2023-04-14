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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QWheelEvent>
#include <QWidget>

class ImageViewer : public QGraphicsView {
  Q_OBJECT

 public:
  ImageViewer(QWidget *parent = nullptr);
  ImageViewer &operator=(const ImageViewer &T) = delete;
  ImageViewer &operator=(ImageViewer &&T) = delete;
  ImageViewer(ImageViewer &&T) = delete;
  ~ImageViewer() = default;
  void setImage(const QImage &image);
  void fitToView();
  void clear();
  void setRectangle(const QRect &rect);

 private:
  QGraphicsScene *scene;
  QGraphicsPixmapItem *image;
  QGraphicsRectItem *rectangle;
  double factor;
  double currentZoom;
  QPoint panReferenceClick;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
};
#endif  // IMAGEVIEWER_H
