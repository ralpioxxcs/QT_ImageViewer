#include "qroiView.h"

#include <QBitmap>
#include <QCursor>
#include <QDebug>
#include <QFile>
#include <QMouseEvent>
#include <QToolTip>

#include "logger.h"

using namespace AprLogger;

static const unsigned int zoomInCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xff3ff8ff, 0xff0fe0ff, 0xff07c0ff,
    0xff07c0ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff,
    0xff07c0ff, 0xff07c0ff, 0xff01e0ff, 0xff30f8ff, 0x7ff0ffff, 0x3ff8ffff,
    0x1ffcffff, 0x0ffeffff, 0x0fffffff, 0x9fffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int zoomInCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00c00700, 0x00f01f00,
    0x00f01e00, 0x00f83e00, 0x00f83e00, 0x00183000, 0x00f83e00, 0x00f83e00,
    0x00f01e00, 0x00f01f00, 0x00c00700, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000};

static const unsigned int zoomOutCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xff3ff8ff, 0xff0fe0ff, 0xff07c0ff,
    0xff07c0ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff,
    0xff07c0ff, 0xff07c0ff, 0xff01e0ff, 0xff30f8ff, 0x7ff0ffff, 0x3ff8ffff,
    0x1ffcffff, 0x0ffeffff, 0x0fffffff, 0x9fffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int zoomOutCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00c00700, 0x00f01f00,
    0x00f01f00, 0x00f83f00, 0x00f83f00, 0x00183000, 0x00f83f00, 0x00f83f00,
    0x00f01f00, 0x00f01f00, 0x00c00700, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000};

static const unsigned int noZoomCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xff3ff8ff, 0xff0fe0ff, 0xff07c0ff,
    0xff07c0ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff, 0xff0380ff,
    0xff07c0ff, 0xff07c0ff, 0xff01e0ff, 0xff30f8ff, 0x7ff0ffff, 0x3ff8ffff,
    0x1ffcffff, 0x0ffeffff, 0x0fffffff, 0x9fffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int noZoomCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00c00700, 0x00f01f00,
    0x00f01f00, 0x00f83f00, 0x00f83f00, 0x00f83f00, 0x00f83f00, 0x00f83f00,
    0x00f01f00, 0x00f01f00, 0x00c00700, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000};

static const unsigned int noMove2DCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xff7ffeff, 0xff3ffcff, 0xff1ff8ff,
    0xff0ff0ff, 0xff07e0ff, 0xff03c0ff, 0xff03c0ff, 0xfffc3fff, 0x7ffc3ffe,
    0x3ffc3ffc, 0x1f7c3ef8, 0x0f3c3cf0, 0x071c38e0, 0x071c38e0, 0x0f3c3cf0,
    0x1f7c3ef8, 0x3ffc3ffc, 0x7ffc3ffe, 0xfffc3fff, 0xff03c0ff, 0xff03c0ff,
    0xff07e0ff, 0xff0ff0ff, 0xff1ff8ff, 0xff3ffcff, 0xff7ffeff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int noMove2DCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00800100, 0x00400200, 0x00200400,
    0x00100800, 0x00081000, 0x00042000, 0x00fc3f00, 0x0003c000, 0x80024001,
    0x40024002, 0x20824104, 0x10424208, 0x08224410, 0x08224410, 0x10424208,
    0x20824104, 0x40024002, 0x80024001, 0x0003c000, 0x00fc3f00, 0x00042000,
    0x00081000, 0x00100800, 0x00200400, 0x00400200, 0x00800100, 0x00000000,
    0x00000000, 0x00000000};

static const unsigned int noMoveHorizCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xfffc3fff, 0x7ffc3ffe,
    0x3ffc3ffc, 0x1f7c3ef8, 0x0f3c3cf0, 0x071c38e0, 0x071c38e0, 0x0f3c3cf0,
    0x1f7c3ef8, 0x3ffc3ffc, 0x7ffc3ffe, 0xfffc3fff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int noMoveHorizCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0003c000, 0x80024001,
    0x40024002, 0x20824104, 0x10424208, 0x08224410, 0x08224410, 0x10424208,
    0x20824104, 0x40024002, 0x80024001, 0x0003c000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000};

static const unsigned int noMoveVertCursorA[] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xff7ffeff, 0xff3ffcff, 0xff1ff8ff,
    0xff0ff0ff, 0xff07e0ff, 0xff03c0ff, 0xff03c0ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xff7ffeff, 0xff3ffcff, 0xff1ff8ff, 0xff1ff8ff, 0xff3ffcff,
    0xff7ffeff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff03c0ff, 0xff03c0ff,
    0xff07e0ff, 0xff0ff0ff, 0xff1ff8ff, 0xff3ffcff, 0xff7ffeff, 0xffffffff,
    0xffffffff, 0xffffffff};

static const unsigned int noMoveVertCursorB[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00800100, 0x00400200, 0x00200400,
    0x00100800, 0x00081000, 0x00042000, 0x00fc3f00, 0x00000000, 0x00000000,
    0x00000000, 0x00800100, 0x00400200, 0x00200400, 0x00200400, 0x00400200,
    0x00800100, 0x00000000, 0x00000000, 0x00000000, 0x00fc3f00, 0x00042000,
    0x00081000, 0x00100800, 0x00200400, 0x00400200, 0x00800100, 0x00000000,
    0x00000000, 0x00000000};

static QCursor *hZoomInCursor = 0;
static QCursor *hZoomOutCursor = 0;
static QCursor *hNoZoomCursor = 0;
static QCursor *hNoMove2DCursor = 0;
static QCursor *hNoMoveHorizCursor = 0;
static QCursor *hNoMoveVertCursor = 0;

static QCursor *createCursor(const unsigned int *andPlane,
                             const unsigned int *orPlane, int hotX, int hotY) {
  // The cursor bitmaps are in and/or plane formats used in the Win32
  // CreateCursor function. We need to change it to the bitmap/mask format used
  // by QT.
  unsigned int buffer[32];
  for (int i = 0; i < 32; ++i)
    buffer[i] = ~(orPlane[i] + andPlane[i]);
  QBitmap bitmap =
      QBitmap::fromData(QSize(32, 32), (uchar *)buffer, QImage::Format_Mono);
  for (int i = 0; i < 32; ++i)
    buffer[i] = orPlane[i] | ~andPlane[i];
  QBitmap mask =
      QBitmap::fromData(QSize(32, 32), (uchar *)buffer, QImage::Format_Mono);

  return new QCursor(bitmap, mask, hotX, hotY);
}

static QCursor &getZoomInCursor() {
  if (0 == hZoomInCursor)
    hZoomInCursor = createCursor(zoomInCursorA, zoomInCursorB, 15, 15);
  return *hZoomInCursor;
}

static QCursor &getZoomOutCursor() {
  if (0 == hZoomOutCursor)
    hZoomOutCursor = createCursor(zoomOutCursorA, zoomOutCursorB, 15, 15);
  return *hZoomOutCursor;
}

static QCursor &getNoZoomCursor() {
  if (0 == hNoZoomCursor)
    hNoZoomCursor = createCursor(noZoomCursorA, noZoomCursorB, 15, 15);
  return *hNoZoomCursor;
}

static QCursor &getNoMove2DCursor() {
  if (0 == hNoMove2DCursor)
    hNoMove2DCursor = createCursor(noMove2DCursorA, noMove2DCursorB, 15, 15);
  return *hNoMove2DCursor;
}

static QCursor &getNoMoveHorizCursor() {
  if (0 == hNoMoveHorizCursor)
    hNoMoveHorizCursor =
        createCursor(noMoveHorizCursorA, noMoveHorizCursorB, 15, 15);
  return *hNoMoveHorizCursor;
}

static QCursor &getNoMoveVertCursor() {
  if (0 == hNoMoveVertCursor)
    hNoMoveVertCursor =
        createCursor(noMoveVertCursorA, noMoveVertCursorB, 15, 15);
  return *hNoMoveVertCursor;
}

//
// QRoiViewer
//

#define VIEWPORT_MARGIN_X 20
#define VIEWPORT_MARGIN_Y 20
#define VIEWPORT_WIDTH 720
#define VIEWPORT_HEIGHT 540

QRoiView::QRoiView(QWidget *parent) : QLabel(parent) {
  m_viewport = new QViewPort(this);
  m_viewport->setGeometry(QRect(VIEWPORT_MARGIN_X, VIEWPORT_MARGIN_Y,
                                VIEWPORT_WIDTH, VIEWPORT_HEIGHT));
  m_viewport->setFrameShape(QFrame::StyledPanel);
  m_viewport->setLineWidth(5);
  connect(m_viewport, SIGNAL(mouseMoveArea(QMouseEvent *)),
          SLOT(onMouseMove(QMouseEvent *)));
  connect(m_viewport, SIGNAL(mousePressArea(QMouseEvent *)),
          SLOT(onMousePress(QMouseEvent *)));
  connect(m_viewport, SIGNAL(mouseReleaseArea(QMouseEvent *)),
          SLOT(onMouseRelease(QMouseEvent *)));
  connect(m_viewport, SIGNAL(mouseDoubleClickArea(QMouseEvent *)),
          SLOT(onMouseDoubleClick(QMouseEvent *)));

  m_mouseUsage = ZoomGroup::MouseUsageDefault;
  m_zoomDirection = ZoomGroup::DirectionHorizontal;
  m_scrollDirection = ZoomGroup::DirectionHorizontal;
  m_zoomInRatio = 2;
  m_zoomOutRatio = 0.5;
  m_mouseWheelZoomRatio = 1.1;

  m_minDragAmount = 5;
  m_isOnArea = false;
  m_currentPos = QPoint(-1, -1);
  m_isAreaMouseDown = false;
  m_AreaMouseDownPos = QPoint(-1, -1);

  m_selectBoxLineWidth = 2;
  m_selectBoxLineColor = QColor(0, 0, 0);
  m_leftLine = 0;
  m_rightLine = 0;
  m_topLine = 0;
  m_bottomLine = 0;

  m_crossHairFg = nullptr;
  m_bDrawCrossHair = false;
  m_crosshairLineWidth = 1;
  m_crosshairLineColor = QColor(Qt::white);

  m_xPosFg = nullptr;
  m_yPosFg = nullptr;
  m_markerTextFont = QFont(QFont("Arial", 10));
  QFontMetrics fm(m_markerTextFont);
  // QString testText =
  //     QString("p : %1\nsubP : %2").arg(100).arg(100.123, 0, 'f', 3);
  QString testText = QString("p : %1").arg(100.123, 0, 'f', 3);
  m_markerRect = QRect(0, 0, fm.width(testText), fm.height());
  m_bDrawCoordinates = false;
  m_markerText = QColor(Qt::black);
  m_markerBackground = QColor(Qt::yellow);
}

QRoiView::~QRoiView() {
  if (m_viewport != nullptr) {
    delete m_viewport;
  }
}

void QRoiView::setRoi(QImage *i) {
  if (0 != i) {
    m_viewport->updateImage(i);
  }
  return;
}

const QImage *QRoiView::getRoi() {
  const QImage ret = QImage(m_viewport->getImage());
  return &ret;
}

void QRoiView::setSelectionBoxBorderWidth(int width) {
  m_selectBoxLineWidth = width;
}

int QRoiView::getSelectionBoxBorderWidth() { return m_selectBoxLineWidth; }

void QRoiView::setSelectionBorderBoxColor(QColor color) {
  m_selectBoxLineColor = color;
  if (m_topLine != 0) {
    QPalette p(color, color);
    m_topLine->setPalette(p);
    m_bottomLine->setPalette(p);
    m_rightLine->setPalette(p);
    m_leftLine->setPalette(p);
  }
}

QColor QRoiView::getSelectionBorderBoxColor() { return m_selectBoxLineColor; }

void QRoiView::setCrossHairBorderWidth(int width) {
  m_crosshairLineWidth = width;
}

int QRoiView::getCrossHairBorderWidth() { return m_crosshairLineWidth; }

void QRoiView::setCrossHairBorderColor(QColor color) {
  m_crosshairLineColor = color;
}

QColor QRoiView::getCrossHairBorderColor() { return m_crosshairLineColor; }

void QRoiView::setMouseUsage(int mouseUsage) {
  m_mouseUsage = mouseUsage;

  if (mouseUsage != ZoomGroup::MouseUsageScroll) {
    m_viewport->setDragEnable(false);
  } else if (mouseUsage == ZoomGroup::MouseUsageScroll) {
    m_viewport->setDragEnable(true);
  }
}

int QRoiView::getMouseUsage() { return m_mouseUsage; }

void QRoiView::setZoomDirection(int direction) { m_zoomDirection = direction; }

int QRoiView::getZoomDirection() { return m_zoomDirection; }

void QRoiView::setScrollDirection(int direction) {
  m_scrollDirection = direction;
}

int QRoiView::getScrollDirection() { return m_scrollDirection; }

void QRoiView::setZoomInRatio(double ratio) { m_zoomInRatio = ratio; }

double QRoiView::getZoomInRatio() { return m_zoomInRatio; }

void QRoiView::setZoomOutRatio(double ratio) { m_zoomOutRatio = ratio; }

double QRoiView::getZoomOutRatio() { return m_zoomOutRatio; }

void QRoiView::setMouseWheelZoomRatio(double ratio) {
  m_mouseWheelZoomRatio = ratio;
}

double QRoiView::getMouseWheelZoomRatio() { return m_mouseWheelZoomRatio; }

//
// QRoiViewer Slots
//
void QRoiView::onMouseMove(QMouseEvent *ev) {
  m_isOnArea = true;

  m_currentPos.setX(m_viewport->getHotSpotPoint().x());
  m_currentPos.setY(m_viewport->getHotSpotPoint().y());

  // CrossHair ( only pointer mode )
  if (m_mouseUsage == ZoomGroup::MouseUsageScroll) {
    m_bDrawCrossHair = true;
    setCrossHairVisible(true);
    setMarkerVisible(true);
  }

  // 드래그 하는경우 Rect
  if (m_isAreaMouseDown && m_mouseUsage != ZoomGroup::MouseUsageScroll) {
    onMouseDrag();
  }
  onSetCursor();
}

void QRoiView::onMousePress(QMouseEvent *ev) {
  if (ev->button() == Qt::LeftButton &&
      (m_mouseUsage != ZoomGroup::MouseUsageDefault)) {
    /*
     * 마우스 활성은 확대/축소에서 드래그를 하기 위해 사용
     * 사각형 선택영역을 그려주기 위해 마우스 아래 방향을 저장하고
     * 마우스 드래그 시작점 저장한다
     */
    m_isAreaMouseDown = true;
    m_AreaMouseDownPos.setX(m_viewport->getHotSpotPoint().x());
    m_AreaMouseDownPos.setY(m_viewport->getHotSpotPoint().y());
  }
}

void QRoiView::onMouseRelease(QMouseEvent *ev) {
  if ((ev->button() == Qt::LeftButton) && m_isAreaMouseDown) {
    m_isAreaMouseDown = false;
    setRectVisible(false);
    bool hasUpdate = false;

    switch (m_mouseUsage) {
    case ZoomGroup::MouseUsageZoomIn:
      if (canZoomIn()) {
        // 드래그 한 경우 ( Rect )
        if (isDrag(m_zoomDirection, m_currentPos)) {
          hasUpdate = zoomTo();
        }
        // 점 찍은 경우 ( Click )
        else {
          hasUpdate = zoomTo();
        }
      }
      break;
    case ZoomGroup::MouseUsageZoomOut:
      if (canZoomOut()) {
        hasUpdate = zoomAt();
      }
      break;
    // * 기본
    default:
      break;
    }
  }
  onSetCursor();
}

void QRoiView::onMouseDoubleClick(QMouseEvent *ev) {
  emit mouseDoubleClicko(m_viewport->getSubPoint());
}

void QRoiView::paintEvent(QPaintEvent *paintEventInfo) {

  // Crosshair condition
  if (m_bDrawCrossHair == true) {
    drawCrossHair(m_currentPos.x(), m_currentPos.y(), m_viewport->width(),
                  m_viewport->height());
    QPoint labelPos;
    labelPos.setX(m_viewport->getHotSpotPoint().x());
    labelPos.setY(m_viewport->getHotSpotPoint().y());
    drawMarker(m_viewport->getPoint(), m_viewport->getSubPoint(), labelPos);
  }

  QLabel::paintEvent(paintEventInfo);
}

void QRoiView::leaveEvent(QEvent *ev) {
  setCrossHairVisible(false);
  setMarkerVisible(false);

  QLabel::leaveEvent(ev);
}

void QRoiView::onSetCursor() {
  // 화면 위에 있는 경우
  if (m_isOnArea) {
    switch (m_mouseUsage) {
    case ZoomGroup::MouseUsageZoomIn:
      if (canZoomIn()) {
        setCursor(getZoomInCursor());
      } else {
        setCursor(getNoZoomCursor());
      }
      return;
    case ZoomGroup::MouseUsageZoomOut:
      if (canZoomOut()) {
        setCursor(getZoomOutCursor());
      } else {
        setCursor(getNoZoomCursor());
      }
      return;
    // case ZoomGroup::MouseUsageScroll:
    //   return;
    default:
      unsetCursor();
      return;
    }
  }
}

bool QRoiView::isDrag(int direction, QPoint pos) {
  int spanX = abs(pos.x() - m_AreaMouseDownPos.x());
  int spanY = abs(pos.y() - m_AreaMouseDownPos.y());
  return ((direction != ZoomGroup::DirectionVertical) &&
          (spanX >= m_minDragAmount)) ||
         ((direction != ZoomGroup::DirectionHorizontal) &&
          (spanY >= m_minDragAmount));
}

void QRoiView::onMouseDrag() {
  switch (m_mouseUsage) {
  case ZoomGroup::MouseUsageZoomIn: {
    bool isDragZoom = isDrag(m_zoomDirection, m_currentPos);
    if (isDragZoom) {
      int spanX = m_AreaMouseDownPos.x() - m_currentPos.x();
      int spanY = m_AreaMouseDownPos.y() - m_currentPos.y();
      drawRect(m_currentPos.x(), m_currentPos.y(), spanX, spanY);
    }
    setRectVisible(isDragZoom);
    break;
  }
  }
}

void QRoiView::initRect() {
  m_leftLine = new QLabel(this);
  m_leftLine->setAutoFillBackground(true);
  m_rightLine = new QLabel(this);
  m_rightLine->setAutoFillBackground(true);
  m_topLine = new QLabel(this);
  m_topLine->setAutoFillBackground(true);
  m_bottomLine = new QLabel(this);
  m_bottomLine->setAutoFillBackground(true);

  setSelectionBorderBoxColor(getSelectionBorderBoxColor());
}

void QRoiView::drawRect(int x, int y, int width, int height) {

  // Create the edges of the rectangle if not already created
  if (m_topLine == 0) {
    initRect();
  }

  // width < 0 is interpreted as the rectangle extends to the left or x.
  // height <0 is interpreted as the rectangle extends to above y.
  if (width < 0) {
    x -= (width = -width);
  }
  if (height < 0) {
    y -= (height = -height);
  }

  // Put the edges along the sides of the rectangle
  m_topLine->move(x, y);
  m_topLine->setFixedSize(width, m_selectBoxLineWidth);
  m_leftLine->move(x, y);
  m_leftLine->setFixedSize(m_selectBoxLineWidth, height);
  m_bottomLine->move(x, y + height - m_selectBoxLineWidth + 1);
  m_bottomLine->setFixedSize(width, m_selectBoxLineWidth);
  m_rightLine->move(x + width - m_selectBoxLineWidth + 1, y);
  m_rightLine->setFixedSize(m_selectBoxLineWidth, height);
}

void QRoiView::setRectVisible(bool b) {
  // Create the edges of the rectangle if not already created
  if (b && (m_topLine == 0)) {
    initRect();
  }

  // Show/hide the edges
  if (m_topLine != 0) {
    m_topLine->setVisible(b);
    m_leftLine->setVisible(b);
    m_bottomLine->setVisible(b);
    m_rightLine->setVisible(b);
  }
}

void QRoiView::initCrossHair() {
  m_crossHairFg = new QLabel(this);
  m_crossHairFg->setGeometry(VIEWPORT_MARGIN_X, VIEWPORT_MARGIN_Y,
                             VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  m_crossHairFg->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_crossHairFg->setAttribute(Qt::WA_TranslucentBackground);

  setCrossHairBorderColor(getCrossHairBorderColor());
}

void QRoiView::drawCrossHair(int x, int y, int ex, int ey) {

  if (m_crossHairFg == nullptr) {
    initCrossHair();
  }

  QPixmap tmp(m_crossHairFg->width(), m_crossHairFg->height());
  tmp.fill(Qt::transparent);
  QPainter painter(&tmp);
  QPen pen(m_crosshairLineColor, 1, Qt::DashLine);
  painter.setPen(pen);
  painter.drawLine(m_currentPos.x(), 0, m_currentPos.x(), ey);
  painter.drawLine(0, m_currentPos.y(), ex, m_currentPos.y());

  m_crossHairFg->setPixmap(tmp);
}

void QRoiView::setCrossHairVisible(bool b) {
  if (m_crossHairFg != nullptr) {
    if (b == false) {
      m_crossHairFg->hide();
    } else {
      m_crossHairFg->show();
    }
    update();
  }
}

void QRoiView::initMarker() {
  if (m_xPosFg == nullptr) {
    m_xPosFg = new QLabel(this);
    m_xPosFg->setFixedSize(m_markerRect.width(), m_markerRect.height());
  }
  if (m_yPosFg == nullptr) {
    m_yPosFg = new QLabel(this);
    m_yPosFg->setFixedSize(m_markerRect.width(), m_markerRect.height());
  }
}

void QRoiView::drawMarker(QPoint pos, QPointF fpos, QPoint labelPos) {
  if (m_xPosFg == nullptr || m_yPosFg == nullptr) {
    initMarker();
  }

  QPixmap xpix(m_xPosFg->width(), m_xPosFg->height());
  QPixmap ypix(m_yPosFg->width(), m_yPosFg->height());
  xpix.fill(Qt::yellow);
  ypix.fill(Qt::yellow);

  QPainter painterx(&xpix);
  QPainter paintery(&ypix);
  painterx.setFont(m_markerTextFont);
  paintery.setFont(m_markerTextFont);
  // painterx.drawText(
  //     m_markerRect, Qt::AlignCenter,
  //     QString("x : %1\nsubX : %2").arg(pos.x()).arg(fpos.x(), 0, 'f', 3));
  // paintery.drawText(
  //     m_markerRect, Qt::AlignCenter,
  //     QString("y : %1\nsubY : %2").arg(pos.y()).arg(fpos.y(), 0, 'f', 3));
  painterx.drawText(m_markerRect, Qt::AlignCenter,
                    QString("x : %1").arg(fpos.x(), 0, 'f', 3));
  paintery.drawText(m_markerRect, Qt::AlignCenter,
                    QString("y : %1").arg(fpos.y(), 0, 'f', 3));

  m_xPosFg->setPixmap(xpix);
  m_yPosFg->setPixmap(ypix);

  m_xPosFg->move(labelPos.x(), 0);
  m_yPosFg->move(0, labelPos.y());
}

void QRoiView::setMarkerVisible(bool b) {
  if (m_yPosFg != nullptr && m_xPosFg != nullptr) {
    if (b == false) {
      m_xPosFg->hide();
      m_yPosFg->hide();
    } else {
      m_xPosFg->show();
      m_yPosFg->show();
    }
    update();
  }
}

bool QRoiView::canZoomIn() { return true; }

bool QRoiView::canZoomOut() { return true; }

bool QRoiView::zoomTo() {
  m_viewport->zoomIn();
  return true;
}

bool QRoiView::zoomAt() {
  m_viewport->zoomOut();
  return true;
}

bool QRoiView::zoomAround() { return true; }

bool QRoiView::dragTo() { return true; }
