#include "myqlabel.h"
#include <QDebug>

myQlabel::myQlabel(QWidget *parent) : QLabel(parent) , m_scale(1)
{
    ;
}

void myQlabel::wheelEvent(QWheelEvent *ev)
{
    m_degree = ev->delta ()/8;
    emit Mouse_Wheeling();
}

void myQlabel::mouseMoveEvent(QMouseEvent *ev)
{
    this->x = ev->x();
    this->y = ev->y();
    if(m_dragging) {
        this->end_x = ev->x();
        this->end_y = ev->y();
    }
    emit Mouse_Pos();
}

void myQlabel::mousePressEvent(QMouseEvent *ev)
{
    this->start_x = ev->x();
    this->start_y = ev->y();
    this->m_dragging = true;
    emit Mouse_Pressed();
}

void myQlabel::mouseReleaseEvent(QMouseEvent *ev)
{
    this->end_x = ev->x();
    this->end_y = ev->y();
    emit Mouse_Released();
}

void myQlabel::leaveEvent(QEvent *)
{
    emit Mouse_Left();
}

