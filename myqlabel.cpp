#include "myqlabel.h"


myQlabel::myQlabel(QWidget *parent) : QLabel(parent) , m_scale(1)
{
    ;
}

void myQlabel::wheelEvent(QWheelEvent *ev)
{
    printf("delta:%f\n",ev->delta());
    m_scale = (ev->delta()/120);
    emit Mouse_Wheeling();
}

void myQlabel::mouseMoveEvent(QMouseEvent *ev)
{
    this->x = ev->x();
    this->y = ev->y();
    emit Mouse_Pos();
}

void myQlabel::mousePressEvent(QMouseEvent *ev)
{
    this->x = ev->x();
    this->y = ev->y();
    emit Mouse_Pressed();
}

void myQlabel::leaveEvent(QEvent *)
{
    emit Mouse_Left();
}

