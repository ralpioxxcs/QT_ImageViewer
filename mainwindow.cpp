#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label_image, SIGNAL(Mouse_Pos()), this, SLOT(mouseMoved()));
    connect(ui->label_image, SIGNAL(Mouse_Wheeling()), this, SLOT(mouseWheel()));

    m_scaleImage = 1.0f;
    QImage image("C:/test.jpg");
    ui->label_image->setPixmap(QPixmap::fromImage(image));
    ui->label_image->setScaledContents(true);

    m_scroller = new QScrollArea;
    m_scroller->setBackgroundRole(QPalette::Dark);
    m_scroller->setWidget(ui->label_image);
    m_scroller->setVisible(false);

    m_scroller->show();

}

MainWindow::~MainWindow()
{
    delete ui;
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
    std::cout<<"wW\n";
}

void MainWindow::mouseWheel()
{
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
