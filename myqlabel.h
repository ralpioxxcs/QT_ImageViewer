#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWidget>
#include <QDrag>

class myQlabel : public QLabel {
  Q_OBJECT
 public:
  explicit myQlabel(QWidget* parent = 0);

  void wheelEvent(QWheelEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;
  void mousePressEvent(QMouseEvent* ev) override;
  void mouseReleaseEvent(QMouseEvent* ev) override;
  void leaveEvent(QEvent*);

  int x, y;
  double m_scale;

  int m_degree;

  int start_x, start_y;
  int end_x, end_y;
  bool m_dragging;

 signals:
  void Mouse_Pressed();
  void Mouse_Released();
  void Mouse_Pos();
  void Mouse_Left();
  void Mouse_Wheeling();
};

#endif  // MYQLABEL_H
