#include "zoomDialog.h"

#include <unistd.h>

#include <QButtonGroup>
#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <sstream>

#include "logger.h"

using namespace AprLogger;

ZoomDialog::ZoomDialog(QWidget *parent) : QDialog(parent) {

  // Set Window
  // 전체 윈도우 설정
  setWindowTitle("Image Zoommer");
  setFixedSize(920, 600);

  // Set Style Panel
  // 스타일 패널 설정
  QFrame *frame = new QFrame(this);
  frame->setGeometry(10, 10, 120, 580);
  frame->setFrameShape(QFrame::StyledPanel);

  // Set Pointer Button
  // 포인터 버튼 설정
  QPushButton *pointerPB =
      new QPushButton(QIcon(":/pointer.png"), "Pointer", frame);
  pointerPB->setGeometry(4, 8, 112, 28);
  pointerPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
  pointerPB->setCheckable(true);

  // Set Zoom Button (In)
  // 확대 버튼 설정
  QPushButton *zoomInPB =
      new QPushButton(QIcon(":/zoomin.png"), "Zoom In", frame);
  zoomInPB->setGeometry(4, 36, 112, 28);
  zoomInPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
  zoomInPB->setCheckable(true);

  // Set Zoom Button (Out)
  // 축소 버튼 설정
  QPushButton *zoomOutPB =
      new QPushButton(QIcon(":/zoomout.png"), "Zoom Out", frame);
  zoomOutPB->setGeometry(4, 64, 112, 28);
  zoomOutPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
  zoomOutPB->setCheckable(true);

  // Set Save Button
  // 저장 버튼 설정
  QPushButton *savePB = new QPushButton(QIcon(":/save.png"), "Save", frame);
  savePB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
  savePB->setGeometry(4, 120, 112, 28);
  connect(savePB, SIGNAL(clicked(bool)), SLOT(onSave(bool)));

  // Set Load Button
  // 불러오기 버튼 설정
  QPushButton *loadPB = new QPushButton(QIcon(":/add.png"), "Load", frame);
  loadPB->setStyleSheet("QPushButton { text-align:left; padding:5px}");
  loadPB->setGeometry(4, 148, 112, 28);
  connect(loadPB, SIGNAL(clicked(bool)), SLOT(onLoad(bool)));

  // The Pointer/Zoom In/Zoom Out buttons form a button group
  // 포인터, 확대, 축소 버튼 그룹 생성
  QButtonGroup *mouseUsage = new QButtonGroup(frame);
  mouseUsage->addButton(pointerPB, ZoomGroup::MouseUsageScroll);
  mouseUsage->addButton(zoomInPB, ZoomGroup::MouseUsageZoomIn);
  mouseUsage->addButton(zoomOutPB, ZoomGroup::MouseUsageZoomOut);
  connect(mouseUsage, SIGNAL(buttonPressed(int)),
          SLOT(onMouseUsageChanged(int)));

  // Zoom Mode label
  // 줌 레벨 라벨
  QLabel *zoomModeLabel = new QLabel("Zoom Level", frame);
  zoomModeLabel->setGeometry(6, 200, 110, 18);
  zoomModeLabel->setAlignment(Qt::AlignHCenter);
  // Zoom level bar
  // 줌 레벨 설정
  m_ZoomBar = new QSlider(Qt::Horizontal, frame);
  m_ZoomBar->setGeometry(12, 225, 102, 41);
  m_ZoomBar->setRange(1, 100);
  m_ZoomBar->setTickPosition(QSlider::TicksBothSides);
  m_ZoomBar->setInvertedAppearance(true);
  connect(m_ZoomBar, SIGNAL(valueChanged(int)), SLOT(onZoomBarChanged(int)));

  // Check Box
  // 체크박스
  m_CheckBox = new QCheckBox("&Use subpixel", frame);
  m_CheckBox->setGeometry(4, 270, 100, 20);
  m_CheckBox->setChecked(true);
  m_bUseSubPixel = true;
  connect(m_CheckBox, SIGNAL(clicked()), this, SLOT(onUseSubPixel()));

  // Pixel Point label
  QLabel *pixelPt = new QLabel("[ Pixel Point ]", frame);
  pixelPt->setGeometry(6, 300, 100, 18);
  pixelPt->setStyleSheet("font-weight:bold");
  pixelPt->setAlignment(Qt::AlignHCenter);

  QLabel *ptX = new QLabel("X :", frame);
  QLabel *ptY = new QLabel("Y :", frame);
  ptX->setGeometry(6, 320, 20, 18);
  ptX->setAlignment(Qt::AlignLeft);
  ptY->setGeometry(6, 340, 20, 18);
  ptY->setAlignment(Qt::AlignLeft);

  m_lineEditX = new QLineEdit(frame);
  m_lineEditY = new QLineEdit(frame);
  m_lineEditX->setGeometry(26, 320, 60, 18);
  m_lineEditY->setGeometry(26, 340, 60, 18);

  ///////////////////////////////////////////////////////////////////////////////

  // Set Image viewer
  // 이미지 뷰어 설정
  m_RoiView = new QRoiView(this);
  m_RoiView->setGeometry(QRect(150, 15, 760, 580));
  connect(m_RoiView, SIGNAL(mouseDoubleClicko(QPointF)), this,
          SLOT(onChangeTextX(QPointF)));
  connect(m_RoiView, SIGNAL(mouseDoubleClicko(QPointF)), this,
          SLOT(onChangeTextY(QPointF)));
  // m_RoiView->setFrameShape(QFrame::WinPanel);
  // m_RoiView->setLineWidth(1);

  setMouseTracking(true);

  // Initially set the mouse to drag to scroll mode
  // 포인터 버튼을 스크롤 모드로 초기화
  pointerPB->click();
}

ZoomDialog::~ZoomDialog() {}

void ZoomDialog::loadImage() {
  //APR_LOGGER(GET_LOGGER, DEBUG) << "ZoomDialog::loadImage()";

  QString fileName = QFileDialog::getOpenFileName(
      this, "Load", "T5-Calibration", "PNG (*.png);;JPG (*.jpg)");

  if (!fileName.isEmpty()) {
    QImage i(fileName);
    i = i.convertToFormat(QImage::Format_Indexed8);
    m_RoiView->setRoi(&i);
  }
}

void ZoomDialog::saveImage() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save", "T5-Calibration", "PNG (*.png);;JPG (*.jpg)");

  if (!fileName.isEmpty()) {
    const QImage *i = m_RoiView->getRoi();
    if (i->isNull() == false) {
      i->save(fileName.toUtf8().constData());
    }
  }
}

void ZoomDialog::updateImage() {}

void ZoomDialog::onMouseUsageChanged(int mouseUsage) {
  m_RoiView->setMouseUsage(mouseUsage);
}

void ZoomDialog::onSave(bool) { saveImage(); }

void ZoomDialog::onLoad(bool) { loadImage(); }

void ZoomDialog::onZoomBarChanged(int value) {
  /*
  if (!m_RoiView->isInViewPortChangedEvent()) {
    // Remember the center point
    double centerX = m_ChartViewer->getViewPortLeft() +
                     m_ChartViewer->getViewPortWidth() / 2;
    double centerY = m_ChartViewer->getViewPortTop() +
                     m_ChartViewer->getViewPortHeight() / 2;

    // Aspect ratio and zoom factor
    double aspectRatio =
        m_ChartViewer->getViewPortWidth() / m_ChartViewer->getViewPortHeight();
    double zoomTo = ((double)value) / m_ZoomBar->maximum();

    // Zoom by adjusting ViewPortWidth and ViewPortHeight while maintaining the
    // aspect ratio
    m_ChartViewer->setViewPortWidth(zoomTo *
                                    ((aspectRatio < 1) ? 1 : aspectRatio));
    m_ChartViewer->setViewPortHeight(
        zoomTo * ((aspectRatio > 1) ? 1 : (1 / aspectRatio)));

    // Adjust ViewPortLeft and ViewPortTop to keep center point unchanged
    m_ChartViewer->setViewPortLeft(centerX -
                                   m_ChartViewer->getViewPortWidth() / 2);
    m_ChartViewer->setViewPortTop(centerY -
                                  m_ChartViewer->getViewPortHeight() / 2);

    // Update the chart image only, but no need to update the image map.
    m_ChartViewer->updateViewPort(true, false);
  }
  */
}

void ZoomDialog::onMouseMoveArea(QMouseEvent *ev) {
  // 마우스 커서를 따라 십자선 그림
  // auto label = m_RoiView->getDrawLayer();

  // QImage draw(label->size(), QImage::Format_ARGB32);
  // draw.fill(Qt::transparent);
  // QPainter painter(&draw);
  // QPen pen(Qt::white, 1, Qt::DashLine);
  // painter.setPen(pen);

  // int x = m_RoiView->getMouseX();
  // int xx = m_RoiView->getImageHeight();

  // // 십자선
  // painter.drawLine(m_RoiView->getMouseX(), 0, m_RoiView->getMouseX(),
  //                  m_RoiView->getImageHeight());
  // painter.drawLine(0, m_RoiView->getMouseY(), m_RoiView->getImageWidth(),
  //                  m_RoiView->getMouseY());

  // m_RoiView->getDrawLayer()->setPixmap(QPixmap::fromImage(draw));
  // drawCrossHair(draw, m_RoiView->getMouseX(), m_RoiView->getMouseY());
}

void ZoomDialog::onClickImage(QMouseEvent *ev) {
  // display current coordinates point
}

void ZoomDialog::onUseSubPixel() {
  if (m_CheckBox->isChecked()) {
    m_bUseSubPixel = true;
  } else {
    m_bUseSubPixel = false;
  }
}

void ZoomDialog::onChangeTextX(QPointF pos) {
  qreal pt_double = pos.x();
  int pt_intger = static_cast<int>(pos.x());
  if (m_bUseSubPixel) {
    m_lineEditX->setText(QString("%1").arg(pt_double, 0, 'f', 3));
  } else {
    m_lineEditX->setText(QString("%1").arg(pt_intger));
  }
}

void ZoomDialog::onChangeTextY(QPointF pos) {
  qreal pt_double = pos.y();
  int pt_intger = static_cast<int>(pos.y());
  if (m_bUseSubPixel) {
    m_lineEditY->setText(QString("%1").arg(pt_double, 0, 'f', 3));
  } else {
    m_lineEditY->setText(QString("%1").arg(pt_intger));
  }
}
