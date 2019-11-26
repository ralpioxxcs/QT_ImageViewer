#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label_image, SIGNAL(Mouse_Pressed()), this, SLOT(mousePressed()));
    connect(ui->label_image, SIGNAL(Mouse_Pos()), this, SLOT(mouseMoved()));
    connect(ui->label_image, SIGNAL(Mouse_Wheeling()), this, SLOT(mouseWheel()));
    connect(ui->label_image, SIGNAL(Mouse_Released()), this, SLOT(mosueReleased()));
    //connect(ui->label_image, SIGNAL(Mouse_Wheeling()), this, SLOT(mouseWheel()));

    m_scaleImage = 1.0f;
    QImage image("C:/test.jpg");
    ui->label_image->setPixmap(QPixmap::fromImage(image));
    //ui->label_image->setGeometry (0,0,image.size().width (),image.size ().height ());
    //ui->label_image->setFixedSize (image.size ());
    ui->label_image->setScaledContents(true);
    ui->label_image->setAlignment (Qt::AlignCenter);
    ui->label_image->adjustSize ();

    m_scroller = new QScrollArea;
    m_scroller->setBackgroundRole(QPalette::Dark);
    m_scroller->setWidget(ui->label_image);
    m_scroller->setVisible(false);
    //m_scroller->setGeometry (0,0,image.size().width (),image.size ().height ());


    m_scroller->show();

    ui->graphicsView->setBackgroundBrush (QImage("C:/test.jpg"));
    ui->graphicsView->setCacheMode (QGraphicsView::CacheBackground);
    ui->graphicsView->show ();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mosueReleased()
{
    int curX,curY;
    curX = ui->label_image->x;
    curY = ui->label_image->y;

//    int shiftX, shiftY;
//    shiftX = curX-m_startPt.x ();
//    shiftY = curY-m_startPt.y ();
//    qDebug()<<"shift x :"<<shiftX;
//    qDebug()<<"shift y :"<<shiftY;
//    qDebug()<<"";

//    qDebug()<<"horizontalScrollBar val :"<<m_scroller->horizontalScrollBar ()->value();
//    qDebug()<<"verticalScrollBar val :"<<m_scroller->verticalScrollBar ()->value();
//    m_scroller->horizontalScrollBar ()->setValue(m_scroller->horizontalScrollBar ()->value()+(-shiftX));
//    m_scroller->verticalScrollBar()->setValue(m_scroller->verticalScrollBar ()->value()+(-shiftY));
}

void MainWindow::mousePressed()
{
    m_startPt.setX (ui->label_image->start_x);
    m_startPt.setY (ui->label_image->start_y);
    qDebug()<<"=================\n m_startPt :"<<m_startPt;

    m_startScrollPt.setX (m_scroller->horizontalScrollBar ()->value());
    m_startScrollPt.setY (m_scroller->verticalScrollBar ()->value());
}

void MainWindow::mouseMoved()
{
    QString txt;
    int x=ui->label_image->x;
    int y=ui->label_image->y;
    x/=m_scaleImage;
    y/=m_scaleImage;
    txt.sprintf(" X : %d , Y : %d ",x, y);
    ui->label_pos->setText(txt);

    int curX,curY;
    curX = ui->label_image->x;
    curY = ui->label_image->y;
    //curX=m_startPt.x()-curX;
    //curY=m_startPt.y()-curY;
    qDebug()<<"cur x :"<<curX;
    qDebug()<<"cur y :"<<curY;
/*    qDebug()<<"cur x - startx :"<<curX;
    qDebug()<<"cur y - starty:"<<curY;
    qDebug()<<"";*/

    m_currentScrollPt.setX (m_scroller->horizontalScrollBar ()->value());
    m_currentScrollPt.setY (m_scroller->verticalScrollBar ()->value());


    int shiftX, shiftY;
    shiftX = curX-m_startPt.x ()-(m_currentScrollPt.x ()-m_startScrollPt.x ());
    shiftY = curY-m_startPt.y ()-(m_currentScrollPt.y ()-m_startScrollPt.y ());
    shiftX/=2;
    shiftY/=2;
    qDebug()<<"shift x :"<<shiftX;
    qDebug()<<"shift y :"<<shiftY;
    qDebug()<<"";

//    qDebug()<<"horizontalScrollBar val :"<<m_scroller->horizontalScrollBar ()->value();
//    qDebug()<<"verticalScrollBar val :"<<m_scroller->verticalScrollBar ()->value();
    m_scroller->horizontalScrollBar ()->setValue((m_scroller->horizontalScrollBar ()->value()-(shiftX)));
    m_scroller->verticalScrollBar()->setValue(m_scroller->verticalScrollBar ()->value()-(shiftY));
    qDebug()<<"horizontalScrollBar val :"<<m_scroller->horizontalScrollBar ()->value();
    qDebug()<<"verticalScrollBar val :"<<m_scroller->verticalScrollBar ()->value();
}

void MainWindow::mouseWheel()
{
    qDebug() <<"wheel event";
    const int degrees = ui->label_image->m_degree;
    qDebug() << degrees;
    int steps = degrees / 15;
    if(steps >0) {
        qDebug() << "zoom in";
        scaleImage(1.03);
    } else {
        qDebug() << "zoom out";
        scaleImage(1.0/1.03);
    }

    double scale = ui->label_image->m_scale;
    scaleImage(scale);
}

void MainWindow::scaleImage(double factor)
{
    m_scaleImage *= factor;

    QString txt;
    txt.sprintf(" scale:%f ",m_scaleImage);
    ui->label_pos->setText(txt);

    QSize size = ui->label_image->pixmap()->size() * m_scaleImage;
    ui->label_image->resize(size);
    ui->label_image->setScaledContents(true);
}

void MainWindow::on_pushButton_clicked()
{
    scaleImage(1.0/1.1);

}

void MainWindow::on_pushButton_2_clicked()
{
    scaleImage(1.1);
}
