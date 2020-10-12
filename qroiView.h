/**
 *  @filename: qroiView.h
 *  @brief :
 *  @author : iaeo94@golfzon.com
 *
 **/

#ifndef QROIVIEW_H
#define QROIVIEW_H

#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QString>
#include <QTransform>
#include <QWheelEvent>
#include <QWidget>

#include "viewport.h"

namespace ZoomGroup {

// 마우스 사용 enum 정의
enum {
  MouseUsageScroll = 0,
  MouseUsageZoomIn,
  MouseUsageZoomOut,
  MouseUsageDefault
} MouseUsage;

enum {
  DirectionHorizontal = 0,
  DirectionVertical,
  DirectionHorizontalVertical
} Direction;

} // namespace ZoomGroup

class QRoiView : public QLabel {
  Q_OBJECT
public:
  explicit QRoiView(QWidget *parent = 0);
  ~QRoiView();

  virtual void setRoi(QImage *i);
  virtual const QImage *getRoi();

  virtual void setSelectionBoxBorderWidth(int width);
  virtual int getSelectionBoxBorderWidth();
  virtual void setSelectionBorderBoxColor(QColor color);
  virtual QColor getSelectionBorderBoxColor();

  virtual void setCrossHairBorderWidth(int width);
  virtual int getCrossHairBorderWidth();
  virtual void setCrossHairBorderColor(QColor color);
  virtual QColor getCrossHairBorderColor();

  virtual void setMouseUsage(int mouseUsage);
  virtual int getMouseUsage();

  virtual void setZoomDirection(int direction);
  virtual int getZoomDirection();
  virtual void setScrollDirection(int direction);
  virtual int getScrollDirection();

  virtual void setZoomInRatio(double ratio);
  virtual double getZoomInRatio();
  virtual void setZoomOutRatio(double ratio);
  virtual double getZoomOutRatio();
  virtual void setMouseWheelZoomRatio(double ratio);
  virtual double getMouseWheelZoomRatio();

public slots:
  virtual void onMouseMove(QMouseEvent *ev);
  virtual void onMousePress(QMouseEvent *ev);
  virtual void onMouseRelease(QMouseEvent *ev);
  virtual void onMouseDoubleClick(QMouseEvent *ev);
  // virtual void onWheel(QMouseEvent *ev);

signals:
  void mouseDoubleClicko(QPointF pos);

protected:
  // void mouseMoveEvent(QMouseEvent *ev) override;
  void paintEvent(QPaintEvent *paintEventInfo) override;
  void leaveEvent(QEvent *ev) override;

  virtual void onSetCursor();

private:
  QViewPort *m_viewport;

  // values
  int m_mouseUsage;             // 마우스 활성
  int m_zoomDirection;          // 확대 방향
  int m_scrollDirection;        // 스크롤 방향
  double m_zoomInRatio;         // 확대 비율
  double m_zoomOutRatio;        // 축소 비율
  double m_mouseWheelZoomRatio; // 마우스 휠 확대, 축소 비율

  // Mouse
  int m_minDragAmount;
  bool m_isOnArea;
  QPoint m_currentPos;
  bool m_isAreaMouseDown;
  QPoint m_AreaMouseDownPos;
  bool isDrag(int direction, QPoint pos);
  void onMouseDrag();

  // Selection Box
  int m_selectBoxLineWidth;
  QColor m_selectBoxLineColor;
  QLabel *m_leftLine;
  QLabel *m_rightLine;
  QLabel *m_topLine;
  QLabel *m_bottomLine;
  void initRect();
  void drawRect(int x, int y, int width, int height);
  void setRectVisible(bool b);

  // CrossHair
  bool m_bDrawCrossHair;
  int m_crosshairLineWidth;
  QColor m_crosshairLineColor;
  QLabel *m_crossHairFg;
  void initCrossHair();
  void drawCrossHair(int x, int y, int ex, int ey);
  void setCrossHairVisible(bool b);

  // Coordinates Marker
  bool m_bDrawCoordinates;
  QLabel *m_xPosFg;
  QLabel *m_yPosFg;
  QFont m_markerTextFont;
  QRect m_markerRect;
  QColor m_markerText;
  QColor m_markerBackground;
  void initMarker();
  void drawMarker(QPoint pos, QPointF fpos, QPoint labelPos);
  void setMarkerVisible(bool b);

  // Util functions
  bool canZoomIn();
  bool canZoomOut();
  bool zoomTo();
  bool zoomAt();
  bool zoomAround();
  bool dragTo();
};

#endif /* ROIVIEW_H */
