#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QDebug>
#include <QScrollBar>
#include <iostream>

#include "myqlabel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mosueReleased();
    void mousePressed();
    void mouseMoved();
    void mouseWheel();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    void scaleImage(double factor);


private:
    Ui::MainWindow *ui;
    QScrollArea *m_scroller;
    double m_scaleImage;

    QPoint m_startPt;


    QPoint m_startScrollPt;
    QPoint m_currentScrollPt;
};

#endif // MAINWINDOW_H
