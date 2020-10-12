#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QGraphicsView>
#include <QImage>
#include <QMouseEvent>
#include <QPointF>
#include <QSize>
#include <QString>
#include <QTransform>
#include <QWheelEvent>
#include <QWidget>
#include <opencv2/opencv.hpp>

class ViewPort;

class ViewPort {
public:
  virtual ~ViewPort() {}

  virtual QWidget *getWidget() = 0;

  virtual double getRatio() = 0;
  virtual void setRatio(int flags) = 0;

  virtual void updateImage(QImage *img) = 0;
};

class QViewPort : public QGraphicsView, public ViewPort {
  Q_OBJECT
public:
  QViewPort(QWidget *parent = 0);
  ~QViewPort() override;

  QWidget *getWidget() override;

  double getRatio() override;
  void setRatio(int flags) override;

  void updateImage(QImage *img) override;

  inline QImage getImage() { return m_imageQt; }
  inline QPointF getHotSpotPoint() { return m_hotspotPosition; }
  inline QPoint getPoint() { return m_mouseCoord; }
  inline QPointF getSubPoint() { return m_fmouseCoord; }

  inline void setDragEnable(bool b) { m_bDragEnable = b; }

protected:
  void paintEvent(QPaintEvent *paintEventInfo) override;

  //
  // Override MouseEvent (QWidget)
  //
  void wheelEvent(QWheelEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *ev) override;
  void mousePressEvent(QMouseEvent *ev) override;
  void mouseReleaseEvent(QMouseEvent *ev) override;
  void mouseDoubleClickEvent(QMouseEvent *ev) override;
  // void leaveEvent(QEvent* ev) override;

public slots:

  void shiftImageLeft();
  void shiftImageRight();
  void shiftImageUp();
  void shiftImageDown();
  void resetZoom();
  void zoomIn();
  void zoomOut();

signals:
  void mouseMoveArea(QMouseEvent *ev);
  void mousePressArea(QMouseEvent *ev);
  void mouseReleaseArea(QMouseEvent *ev);
  void mouseDoubleClickArea(QMouseEvent *ev);

private:
  int m_param_keepRatio;

  QTransform m_matrixWorld;

  cv::Mat *m_imageCv;
  QImage m_imageQt;

  void scaleView(double factor, QPointF center);
  void moveView(QPointF delta);
  bool m_bDragEnable;

  QPoint m_mouseCoord;       // integer pixel point (u,v)
  QPointF m_fmouseCoord;     // float subpixel point
  QPointF m_hotspotPosition; // mouse pointer point of widget
  QRect m_positionCorners;
  QTransform m_matrixWorld_inv;
  float m_ratioX, m_ratioY;

  void drawOverView(QPainter *painter);
  void drawImageGrid(QPainter *painter);
  void draw2D(QPainter *painter);
  void controlImagePosition();
};

#endif /* VIEWPORT_H */
