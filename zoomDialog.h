#ifndef ZOOMDIALOG_H
#define ZOOMDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <vector>

#include "qroiView.h"
class ZoomDialog : public QDialog {
  Q_OBJECT
public:
  ZoomDialog(QWidget *parent = 0);
  ~ZoomDialog();

private:
  QSlider *m_ZoomBar;
  QCheckBox *m_CheckBox;
  QRoiView *m_RoiView;
  bool m_bUseSubPixel;

  QLineEdit *m_lineEditX;
  QLineEdit *m_lineEditY;

  void loadImage();
  void saveImage();
  void updateImage();

  // protected:
  // void mouseDoubleClickEvent(QMouseEvent *ev) override;

private slots:
  void onMouseUsageChanged(int mouseUsage);
  void onSave(bool);
  void onLoad(bool);
  void onZoomBarChanged(int value);
  void onMouseMoveArea(QMouseEvent *ev);
  void onClickImage(QMouseEvent *ev);
  void onUseSubPixel();

  void onChangeTextX(QPointF pos);
  void onChangeTextY(QPointF pos);
};

#endif /* ZOOMDIALOG_H */
