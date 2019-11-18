#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QLabel>

class myQlabel : public QLabel
{
    Q_OBJECT
public:
    explicit myQlabel(QWidget *parent = 0);

    void wheelEvent( QWheelEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void leaveEvent(QEvent *);

    int x,y;
    double m_scale;

 signals:
    void Mouse_Pressed();
    void Mouse_Pos();
    void Mouse_Left();
    void Mouse_Wheeling();
};

#endif // MYQLABEL_H
