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

#include "timeline.h"
#include "ui_timeline.h"

/**
 * @class Timeline
 *
 * @brief Draw a time line with cursor, hover and marker set.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 5.0 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
*/

Timeline::Timeline(QWidget *parent)
    : QWidget(parent), ui(new Ui::Timeline) {
  ui->setupUi(this);

  // Class members
  m_imageNumber = 100;
  m_imageMin = 0;
  m_width = ui->timelineView->width();
  m_offset = 30;
  m_currentIndex = 0;
  m_currentIndexLeft = 0;
  m_scale = 1;
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, [this]() {
    if (isEnabled()) {
      if (m_currentIndexLeft < m_imageNumber) {
        setValue(m_currentIndexLeft + 1);
      }
      else {
        setValue(0);
      }
    }
  });

  // Scene setup
  timelineScene = new QGraphicsScene(this);
  setLayout(m_width, m_imageNumber);
  ui->timelineView->setAttribute(Qt::WA_Hover);
  ui->timelineView->installEventFilter(this);
  ui->timelineView->setScene(timelineScene);

  //Connection
  connect(ui->zoom, &QAbstractSlider::valueChanged, [this](int value) {
    m_scale = value;
    update(m_currentIndex);
    int x = ((m_width * m_scale - m_offset) * m_currentIndexLeft) / m_imageNumber + static_cast<int>(m_offset * 0.5);
    ui->timelineView->horizontalScrollBar()->setValue(x);
  });

  // Keyboard shorcuts
  // AZERTY keyboard shorcuts are set in the ui
  QShortcut *qShortcut = new QShortcut(QKeySequence("q"), this);
  connect(qShortcut, &QShortcut::activated, [this]() { setValue(m_currentIndexLeft - 1); });

  QShortcut *aShortcut = new QShortcut(QKeySequence("a"), this);
  connect(aShortcut, &QShortcut::activated, [this]() { setValue(m_currentIndexLeft - 1); });

  QShortcut *dShortcut = new QShortcut(QKeySequence("d"), this);
  connect(dShortcut, &QShortcut::activated, [this]() { setValue(m_currentIndexLeft + 1); });

  QShortcut *spaceShortcut = new QShortcut(Qt::Key_Space, this);
  connect(spaceShortcut, &QShortcut::activated, [this]() { ui->playButton->animateClick(); });

  //Ui
  QIcon img = QIcon(":/assets/buttons/play.png");
  ui->playButton->setIcon(img);
  ui->playButton->setIconSize(QSize(ui->playButton->width(), ui->playButton->height()));
  connect(ui->playButton, &QPushButton::clicked, this, &Timeline::togglePlay);
}

/**
  * @brief Set the layout of the timeline.
  * @param[in] width The width of the widget.
  * @param[in] imageNumber The number of images.
*/
void Timeline::setLayout(const int width, const int imageNumber) {
  // Ruler
  double step = static_cast<double>(width * m_scale - m_offset) / static_cast<double>(imageNumber);
  for (int i = 0; i < imageNumber + 1; i++) {
    QGraphicsLineItem *line = new QGraphicsLineItem(static_cast<double>(i) * step + m_offset * 0.5, 2, static_cast<double>(i) * step + m_offset * 0.5, 4);
    timelineScene->addItem(line);
  }

  // Miniature box
  QGraphicsRectItem *timelineMiniature = new QGraphicsRectItem(15, 10, width * m_scale - m_offset, 40);
  timelineMiniature->setBrush(QBrush(QColor(0, 0, 255, 127)));
  timelineScene->addItem(timelineMiniature);

  // Cursors
  cursor = new QGraphicsLineItem(0, 5, 0, 40);
  timelineScene->addItem(cursor);
  cursorLeft = new QGraphicsLineItem(m_offset * 0.5, 5, m_offset * 0.5, 40);
  int x = ((width * m_scale - m_offset) * m_currentIndexLeft) / imageNumber + static_cast<int>(m_offset * 0.5);
  QPen cursorLeftPen = QPen(QColor("black"));
  cursorLeftPen.setWidth(2);
  cursorLeft->setPen(cursorLeftPen);
  cursorLeft->setLine(x, 0, x, 40);
  timelineScene->addItem(cursorLeft);

  // Image number display
  indexNumber = new QGraphicsSimpleTextItem();
  timelineScene->addItem(indexNumber);
}

Timeline::~Timeline() {
  delete ui;
}

/**
  * @brief Handle the widget redrawing when resized.
  * @param[in] *event Pointer to the event.
*/
void Timeline::resizeEvent(QResizeEvent *event) {
  update(m_currentIndexLeft);
}

/**
  * @brief Handle the pointer event, click and hover.
  * @param[in] *target Pointer to the target widget.
  * @param[in] *event Pointer to the event.
*/
bool Timeline::eventFilter(QObject *target, QEvent *event) {
  if (m_imageNumber == 0) {
    return false;
  }
  if (!isEnabled()) {
    return false;
  }

  // Move the cursor when the timeline is hovered by the mouse cursor
  if (target == ui->timelineView && event->type() == QEvent::HoverMove) {
    QHoverEvent *hoverEvent = static_cast<QHoverEvent *>(event);
    int x = static_cast<int>(ui->timelineView->mapToScene(hoverEvent->pos()).x());
    int image = ((x - static_cast<int>(m_offset * 0.5)) * m_imageNumber) / (m_width * m_scale - 30);
    if (x >= 15 && x <= m_width * m_scale - static_cast<int>(m_offset * 0.5)) {
      int image = ((x - static_cast<int>(m_offset * 0.5)) * m_imageNumber) / (m_width * m_scale - m_offset);
      setCursorValue(image);
    }
  }
  if (target == ui->timelineView && event->type() == QEvent::HoverLeave) {
    setCursorValue(m_currentIndexLeft);
  }
  // Left click to place left the cursor at a given position
  // Double left click to place marker
  // Left click to remove markers
  if (target == ui->timelineView && event->type() == QEvent::MouseButtonDblClick) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->buttons() == Qt::LeftButton) {
      int x = static_cast<int>(ui->timelineView->mapToScene(mouseEvent->pos()).x());
      int image = ((x - static_cast<int>(m_offset * 0.5)) * m_imageNumber) / (m_width * m_scale - m_offset);
      drawMarker(image);
      markers.append(image);
    }
  }
  if (target == ui->timelineView && event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->buttons() == Qt::LeftButton) {
      int x = static_cast<int>(ui->timelineView->mapToScene(mouseEvent->pos()).x());
      int image = ((x - static_cast<int>(m_offset * 0.5)) * m_imageNumber) / (m_width * m_scale - m_offset);
      x = ((m_width * m_scale - m_offset) * image) / m_imageNumber + static_cast<int>(m_offset * 0.5);
      setValue(image);
    }
    else if (mouseEvent->buttons() == Qt::RightButton) {
      int x = static_cast<int>(ui->timelineView->mapToScene(mouseEvent->pos()).x());
      int image = ((x - static_cast<int>(m_offset * 0.5)) * m_imageNumber) / (m_width * m_scale - m_offset);
      clearMarker(image);
    }
  }
  return false;
}

/**
  * @brief Draw a line marker at a given index.
  * @param[in] index Index.
*/
void Timeline::drawMarker(const int index) {
  int x = ((m_width * m_scale - m_offset) * index) / m_imageNumber + static_cast<int>(m_offset * 0.5);
  QGraphicsLineItem *markerLine = new QGraphicsLineItem(x, 5, x, 40);
  markerLine->setPen(QPen(QColor("blue")));
  timelineScene->addItem(markerLine);
}

/**
  * @brief Delete a line marker at a given index.
  * @param[in] index Index.
*/
void Timeline::clearMarker(const int index) {
  markers.removeAll(index);
  update(m_currentIndexLeft);
}

/**
  * @brief Redraw the widget keeping markers and cursors.
  * @param[in] index Index.
*/
void Timeline::update(const int index) {
  timelineScene->clear();
  m_width = ui->timelineView->width();
  setLayout(m_width, m_imageNumber);
  for (auto &a : markers) {
    drawMarker(a);
  }
  timelineScene->update();
  setValue(index);
}

/**
  * @brief Set the cursor at a given value.
  * @param[in] index Index.
*/
void Timeline::setCursorValue(const int index) {
  if (m_imageNumber == 0) {
    return;
  }

  int x = ((m_width * m_scale - m_offset) * index) / m_imageNumber + static_cast<int>(m_offset * 0.5);
  QPen cursorPen = QPen(QColor("red"));
  cursor->setPen(cursorPen);
  cursor->setLine(x, 10, x, 40);
  indexNumber->setBrush(QBrush(QColor("red")));
  indexNumber->setText(QString::number(index));
  indexNumber->setPos(x - 5, 55);
  m_currentIndex = index;
  emit(valueChanged(m_currentIndex));
}

/**
  * @brief Set the left cursor (left click cursor) at a given value.
  * @param[in] index Index.
*/
void Timeline::setValue(const int index) {
  if ((m_imageNumber == 0) || (index < m_imageMin) | (index > m_imageNumber)) {
    return;
  }

  int x = ((m_width * m_scale - m_offset) * index) / m_imageNumber + static_cast<int>(m_offset * 0.5);
  QPen cursorLeftPen = QPen(QColor("black"));
  cursorLeftPen.setWidth(2);
  cursorLeft->setPen(cursorLeftPen);
  cursorLeft->setLine(x, 10, x, 40);
  indexNumber->setBrush(QBrush(QColor("red")));
  indexNumber->setText(QString::number(index));
  indexNumber->setPos(x - 5, 55);
  m_currentIndexLeft = index;
  emit(valueChanged(m_currentIndexLeft));
}

/**
  * @brief Return the last left value.
*/
int Timeline::value() {
  return m_currentIndexLeft;
}

/**
  * @brief Return the current value.
*/
int Timeline::currentValue() {
  return m_currentIndex;
}

/**
  * @brief Set the maximum value.
  * @param[in] max Maximum value.
*/
void Timeline::setMaximum(const int max) {
  m_imageNumber = max;
}

/**
  * @brief Set the minimum value, currently forced to zero.
  * @param[in] max minimum value.
*/
void Timeline::setMinimum(const int min) {
  m_imageMin = 0;
}

/**
  * @brief Start/Stop the autoplay of the replay.
*/
void Timeline::togglePlay() {
  if (ui->playButton->isChecked()) {
    QIcon img(":/assets/buttons/pause.png");
    ui->playButton->setIcon(img);
    timer->start(1000 / ui->fpsBox->value());
  }
  else if (!ui->playButton->isChecked()) {
    QIcon img(":/assets/buttons/resume.png");
    ui->playButton->setIcon(img);
    timer->stop();
  }
}
