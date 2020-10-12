#include "viewport.h"
#include <QDebug>

static const unsigned int threshold_zoom_image_region = 30;

QViewPort::QViewPort(QWidget *parent) : QGraphicsView(parent) {
  m_param_keepRatio = 0;

  setContentsMargins(0, 0, 0, 0);
  setMinimumSize(1, 1);
  setAlignment(Qt::AlignHCenter);

  setObjectName(QString::fromUtf8("graphicsView"));

  m_mouseCoord = QPoint(-1, -1);
  m_fmouseCoord = QPointF(-1.0, -1.0);
  m_hotspotPosition = QPointF(0, 0);
  m_positionCorners = QRect(0, 0, size().width(), size().height());

  setStyleSheet("QGraphicsView { border-style: none; }");

  // m_imageCv = new cv::Mat(height(), width(), CV_8UC3, cv::Scalar(0));
  m_imageCv = nullptr;

  m_bDragEnable = true;

  setInteractive(true);
  setMouseTracking(true);
}

QViewPort::~QViewPort() {
  if (m_imageCv) {
    delete m_imageCv;
  }
}

QWidget *QViewPort::getWidget() { return this; }

double QViewPort::getRatio() { return m_param_keepRatio; }

void QViewPort::setRatio(int flags) {
  // If flags is valid
  if (flags == 1) {
    m_param_keepRatio = flags;
    updateGeometry();
    viewport()->update();
  }
}

void QViewPort::updateImage(QImage *img) {
  Q_ASSERT(img);

  if (m_imageCv == nullptr && (img->format() == QImage::Format_RGB888)) {
    qDebug() << "QViewPort::read format(rgb888)";
    QImage swapped = img->rgbSwapped();
    m_imageCv = new cv::Mat(swapped.height(), swapped.width(), CV_8UC3,
                            const_cast<uchar *>(swapped.bits()),
                            static_cast<size_t>(swapped.bytesPerLine()));
  } else if (m_imageCv == nullptr &&
             (img->format() == QImage::Format_Indexed8)) {
    qDebug() << "QViewPort::read format(indexed8)";
    m_imageCv = new cv::Mat(img->height(), img->width(), CV_8UC1,
                            const_cast<uchar *>(img->bits()),
                            static_cast<size_t>(img->bytesPerLine()));
  }
  m_imageQt = QImage(*img);
  m_ratioX = width() / float(m_imageCv->cols);
  m_ratioY = height() / float(m_imageCv->rows);

  updateGeometry();

  viewport()->update();
}

//
// QViewPort overrides
//
void QViewPort::paintEvent(QPaintEvent *paintEv) {
  QPainter myPainter(viewport());
  myPainter.setWorldTransform(m_matrixWorld);
  // myPainter.setFont(QFont("Ubuntu mono", 2));

  draw2D(&myPainter);

  // Now disable matrixWorld for overlay display
  myPainter.setWorldMatrixEnabled(false);

  // overlay pixel values if zoomed in far enough
  if (m_matrixWorld.m11() * m_ratioX >= threshold_zoom_image_region &&
      m_matrixWorld.m11() * m_ratioY >= threshold_zoom_image_region) {
    drawImageGrid(&myPainter);
  }

  // in mode zoom/panning
  if (m_matrixWorld.m11() > 1) {
    drawOverView(&myPainter);
  }

  QGraphicsView::paintEvent(paintEv);
}

void QViewPort::wheelEvent(QWheelEvent *ev) {
  qreal pfx, pfy;
  m_matrixWorld_inv.map(ev->x(), ev->y(), &pfx, &pfy);
  m_mouseCoord.rx() = floor(pfx / m_ratioX);
  m_mouseCoord.ry() = floor(pfy / m_ratioY);
  m_fmouseCoord.rx() = pfx;
  m_fmouseCoord.ry() = pfy;

  scaleView(ev->delta() / 240.0, ev->pos());
  viewport()->update();

  QWidget::wheelEvent(ev);
}

void QViewPort::mouseMoveEvent(QMouseEvent *ev) {
  qreal pfx, pfy;
  m_matrixWorld_inv.map(ev->x(), ev->y(), &pfx, &pfy);

  m_mouseCoord.rx() = floor(pfx / m_ratioX);
  m_mouseCoord.ry() = floor(pfy / m_ratioY);
  m_fmouseCoord.rx() = pfx;
  m_fmouseCoord.ry() = pfy;

  QPoint pt = ev->pos();
  QPointF dxy = (pt - m_hotspotPosition) / m_matrixWorld.m11();
  m_hotspotPosition = pt;

  emit mouseMoveArea(ev);

  if (m_matrixWorld.m11() > 1 && ev->buttons() == Qt::LeftButton &&
      m_bDragEnable) {
    // QPoint pt = ev->pos();
    // QPointF dxy = (pt - m_hotspotPosition) / m_matrixWorld.m11();
    // m_hotspotPosition = pt;
    moveView(dxy);
  }

  QWidget::mouseMoveEvent(ev);
}

void QViewPort::mousePressEvent(QMouseEvent *ev) {
  qreal pfx, pfy;
  m_matrixWorld_inv.map(ev->x(), ev->y(), &pfx, &pfy);
  m_mouseCoord.rx() = floor(pfx / m_ratioX);
  m_mouseCoord.ry() = floor(pfy / m_ratioY);
  m_fmouseCoord.rx() = pfx;
  m_fmouseCoord.ry() = pfy;

  emit mousePressArea(ev);

  if (m_matrixWorld.m11() > 1) {
    setCursor(Qt::ClosedHandCursor);
    m_hotspotPosition = ev->pos();
  }

  QWidget::mousePressEvent(ev);
}

void QViewPort::mouseReleaseEvent(QMouseEvent *ev) {
  qreal pfx, pfy;
  m_matrixWorld_inv.map(ev->x(), ev->y(), &pfx, &pfy);
  m_mouseCoord.rx() = floor(pfx / m_ratioX);
  m_mouseCoord.ry() = floor(pfy / m_ratioY);
  m_fmouseCoord.rx() = pfx;
  m_fmouseCoord.ry() = pfy;

  emit mouseReleaseArea(ev);

  if (m_matrixWorld.m11() > 1) {
    setCursor(Qt::OpenHandCursor);
  }

  QWidget::mouseReleaseEvent(ev);
}

void QViewPort::mouseDoubleClickEvent(QMouseEvent *ev) {
  qreal pfx, pfy;
  m_matrixWorld_inv.map(ev->x(), ev->y(), &pfx, &pfy);
  m_mouseCoord.rx() = floor(pfx / m_ratioX);
  m_mouseCoord.ry() = floor(pfy / m_ratioY);
  m_fmouseCoord.rx() = pfx;
  m_fmouseCoord.ry() = pfy;

  emit mouseDoubleClickArea(ev);
}

//
// QViewPort slots
//
void QViewPort::shiftImageLeft() {
  float delta = 2 * width() / (100.0 * m_matrixWorld.m11());
  moveView(QPointF(delta, 0));
}

void QViewPort::shiftImageRight() {
  float delta = -2 * width() / (100.0 * m_matrixWorld.m11());
  moveView(QPointF(delta, 0));
}

void QViewPort::shiftImageUp() {
  float delta = 2 * height() / (100.0 * m_matrixWorld.m11());
  moveView(QPointF(delta, 0));
}

void QViewPort::shiftImageDown() {
  float delta = -2 * height() / (100.0 * m_matrixWorld.m11());
  moveView(QPointF(delta, 0));
}

void QViewPort::resetZoom() {
  m_matrixWorld.reset();
  controlImagePosition();
}

void QViewPort::zoomIn() {
  scaleView(0.5, QPointF(size().width() / 2, size().height() / 2));
  viewport()->update();
}

void QViewPort::zoomOut() {
  scaleView(-0.5, QPointF(size().width() / 2, size().height() / 2));
  viewport()->update();
}

//
// QViewPort private functions
//
void QViewPort::scaleView(double factor, QPointF center) {
  factor /= 5; //-0.1 <-> 0.1
  factor += 1; // 0.9 <-> 1.1

  // limit zoom out ---
  if (m_matrixWorld.m11() == 1 && factor < 1) {
    return;
  }

  if (m_matrixWorld.m11() * factor < 1) {
    factor = 1 / m_matrixWorld.m11();
  }

  // limit zoom int ---
  if (m_matrixWorld.m11() > 100 && factor > 1) {
    return;
  }

  // inverse the transform
  int a, b;
  m_matrixWorld_inv.map(center.x(), center.y(), &a, &b);

  m_matrixWorld.translate(a - factor * a, b - factor * b);
  m_matrixWorld.scale(factor, factor);

  controlImagePosition();

  if (m_matrixWorld.m11() > 1) {
    setCursor(Qt::OpenHandCursor);
  } else {
    unsetCursor();
  }
}

void QViewPort::moveView(QPointF delta) {
  m_matrixWorld.translate(delta.x(), delta.y());
  controlImagePosition();

  viewport()->update();
}

void QViewPort::drawOverView(QPainter *painter) {
  QSize viewSize = size();
  viewSize.scale(width() * 0.15, height() * 0.15, Qt::KeepAspectRatio);

  const int margin = 5;

  // draw the image's location
  painter->setBrush(QColor(0, 0, 0, 127));
  painter->setPen(Qt::darkGreen);
  painter->drawRect(QRect(width() - viewSize.width() - margin, 0,
                          viewSize.width(), viewSize.height()));

  // daw the view's location inside the image
  qreal ratioSize = 1 / m_matrixWorld.m11();
  qreal ratioWindow = (qreal)(viewSize.height()) / (qreal)(size().height());
  painter->setPen(Qt::darkBlue);
  painter->drawRect(QRectF(
      width() - viewSize.width() -
          m_positionCorners.left() * ratioSize * ratioWindow - margin,
      -m_positionCorners.top() * ratioSize * ratioWindow,
      (viewSize.width() - 1) * ratioSize, (viewSize.height() - 1) * ratioSize));
}

void QViewPort::drawImageGrid(QPainter *painter) {

  if (m_imageCv->type() != CV_8UC1 && m_imageCv->type() != CV_8UC3) {
    return;
  }

  double pixel_width = m_matrixWorld.m11() * m_ratioX;
  double pixel_height = m_matrixWorld.m11() * m_ratioY;

  qreal offsetX = m_matrixWorld.dx() / pixel_width;
  offsetX = offsetX - floor(offsetX);
  qreal offsetY = m_matrixWorld.dy() / pixel_height;
  offsetY = offsetY - floor(offsetY);

  QSize view = size();
  QVarLengthArray<QLineF, 30> linesX;
  for (qreal _x = offsetX * pixel_width; _x < view.width(); _x += pixel_width)
    linesX.append(QLineF(_x, 0, _x, view.height()));

  QVarLengthArray<QLineF, 30> linesY;
  for (qreal _y = offsetY * pixel_height; _y < view.height();
       _y += pixel_height)
    linesY.append(QLineF(0, _y, view.width(), _y));

  QFont f = painter->font();
  int original_font_size = f.pointSize();
  // change font size
  // f.setPointSize(4 + (m_matrixWorld.m11() - threshold_zoom_image_region) /
  // 5);
  f.setPixelSize(10 + (pixel_height - threshold_zoom_image_region) / 5);
  painter->setFont(f);

  for (int j = -1; j < height() / pixel_height;
       j++) //-1 because display the pixels top rows left columns
  {
    for (int i = -1; i < width() / pixel_width; i++) //-1
    {
      // Calculate top left of the pixel's position in the viewport (screen
      // space)
      QPointF pos_in_view((i + offsetX) * pixel_width,
                          (j + offsetY) * pixel_height);

      // Calculate top left of the pixel's position in the image (image space)
      QPointF pos_in_image =
          m_matrixWorld_inv.map(pos_in_view); // Top left of pixel in view
      pos_in_image.rx() = pos_in_image.x() / m_ratioX;
      pos_in_image.ry() = pos_in_image.y() / m_ratioY;
      QPoint point_in_image(pos_in_image.x() + 0.5f,
                            pos_in_image.y() + 0.5f); // Add 0.5 for rounding

      QRgb rgbValue;
      if (m_imageQt.valid(point_in_image)) {
        rgbValue = m_imageQt.pixel(point_in_image);
      } else {
        rgbValue = qRgb(0, 0, 0);
      }

      if (m_imageCv->type() == CV_8UC3) {
        // for debug
        /*
        val = tr("%1 %2").arg(point2.x()).arg(point2.y());
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(QRect(point1.x(),point1.y(),param_matrixWorld.m11(),param_matrixWorld.m11()/2),
            Qt::AlignCenter, val);
        */
        QString val;

        val = tr("%1").arg(qRed(rgbValue));
        painter->setPen(QPen(Qt::red, 1));
        painter->drawText(QRect(pos_in_view.x(), pos_in_view.y(), pixel_width,
                                pixel_height / 3),
                          Qt::AlignCenter, val);

        val = tr("%1").arg(qGreen(rgbValue));
        painter->setPen(QPen(Qt::green, 1));
        painter->drawText(QRect(pos_in_view.x(),
                                pos_in_view.y() + pixel_height / 3, pixel_width,
                                pixel_height / 3),
                          Qt::AlignCenter, val);

        val = tr("%1").arg(qBlue(rgbValue));
        painter->setPen(QPen(Qt::blue, 1));
        painter->drawText(QRect(pos_in_view.x(),
                                pos_in_view.y() + 2 * pixel_height / 3,
                                pixel_width, pixel_height / 3),
                          Qt::AlignCenter, val);
      }

      if (m_imageCv->type() == CV_8UC1) {
        QString val = tr("%1").arg(qRed(rgbValue));
        int pixel_brightness_value = qRed(rgbValue);
        int text_brightness_value = 0;

        text_brightness_value = pixel_brightness_value > 127
                                    ? pixel_brightness_value - 127
                                    : 127 + pixel_brightness_value;
        painter->setPen(
            QPen(QColor(text_brightness_value, text_brightness_value,
                        text_brightness_value)));
        painter->drawText(
            QRect(pos_in_view.x(), pos_in_view.y(), pixel_width, pixel_height),
            Qt::AlignCenter, val);
      }
    }
  }

  painter->setPen(QPen(Qt::black, 1));
  painter->drawLines(linesX.data(), linesX.size());
  painter->drawLines(linesY.data(), linesY.size());

  // restore font size
  if (original_font_size > 0) {
    f.setPointSize(original_font_size);
    painter->setFont(f);
  }
}

void QViewPort::draw2D(QPainter *painter) {
  painter->drawImage(QRect(0, 0, viewport()->width(), viewport()->height()),
                     m_imageQt,
                     QRect(0, 0, m_imageQt.width(), m_imageQt.height()));
}

void QViewPort::controlImagePosition() {
  double left, right, top, bottom;

  // Check left top corner
  m_matrixWorld.map(0, 0, &left, &top);
  if (left > 0) {
    m_matrixWorld.translate(-left, 0);
    left = 0;
  }
  if (top > 0) {
    m_matrixWorld.translate(0, -top);
    top = 0;
  }

  // Check right bottom corner
  m_matrixWorld.map(size().width(), size().height(), &right, &bottom);
  if (right < size().width()) {
    m_matrixWorld.translate(size().width() - right, 0);
    right = size().width();
  }
  if (bottom < size().height()) {
    m_matrixWorld.translate(0, size().height() - bottom);
    bottom = size().height();
  }

  m_positionCorners.setTopLeft(QPoint(left, top));
  m_positionCorners.setBottomRight(QPoint(right, bottom));

  m_matrixWorld_inv = m_matrixWorld.inverted();
}
