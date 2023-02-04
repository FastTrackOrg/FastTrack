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

#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), image(new QGraphicsPixmapItem()), rectangle(new QGraphicsRectItem(image)), currentZoom(1), factor(1) {
  this->setBackgroundBrush(QColor("#DEE4E7"));
  scene->addItem(image);
  rectangle->setVisible(false);
  rectangle->setPen(QPen(Qt::green, 5));
  this->setScene(scene);
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void ImageViewer::setRectangle(const QRect &rect) {
  if (rect.isEmpty()) {
    rectangle->setVisible(false);
  }
  else {
    rectangle->setRect(rect);
    rectangle->setVisible(true);
  }
}

void ImageViewer::setImage(const QImage &image) {
  this->clear();
  QPixmap pixmap(QPixmap::fromImage(image));
  this->image->setPixmap(pixmap);
}

void ImageViewer::clear() {
  this->image->setPixmap(QPixmap());
}

void ImageViewer::wheelEvent(QWheelEvent *event) {
  double factor = 0;
  if (event->angleDelta().y() > 0) {
    factor = 1.25;
    this->currentZoom *= factor;
  }
  else {
    factor = 0.8;
    this->currentZoom *= factor;
  }
  this->scale(factor, factor);
  event->accept();
  QGraphicsView::wheelEvent(event);
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MiddleButton) {  // Get pan coordinates reference with middle click
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    this->panReferenceClick = event->pos();
  }
  event->ignore();
  QGraphicsView::mousePressEvent(event);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
  QApplication::restoreOverrideCursor();
  event->ignore();
  QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MiddleButton) {  // pan with middle click pressed
    this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() + (this->panReferenceClick.x() - event->pos().x()));
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() + (this->panReferenceClick.y() - event->pos().y()));
    this->panReferenceClick = event->pos();
  }
  event->ignore();
  QGraphicsView::mouseMoveEvent(event);
}
