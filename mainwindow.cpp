#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("option");
    mdialog1 = new Dialog1;
    mdialog1->setWindowTitle("detailed");
    mdialog2 = new Dialog2;
    mdialog2->setWindowTitle("detailed");

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()    // 필기 o, 사람 o
{
    human = true;
    clean = false;

    mdialog1->show();
    mdialog2->hide();

}


void MainWindow::on_pushButton_2_clicked()    // 필기 x, 사람 o
{
    human = true;
    clean = true;

    mdialog1->show();
    mdialog2->hide();

}


void MainWindow::on_pushButton_3_clicked()    // 필기 o, 사람 x
{
    human = false;
    clean = false;

    mdialog2->show();
    mdialog1->hide();

}


void MainWindow::on_pushButton_4_clicked()    // 필기 x, 사람 x
{
    human = false;
    clean = true;

    mdialog2->show();
    mdialog1->hide();

}

