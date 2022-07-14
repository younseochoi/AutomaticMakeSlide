#include "dialog1.h"
#include "ui_dialog1.h"
#include "mainwindow.h"
//#include <QMessageBox>
#include "code.h"
#include "dialog2.h"
#include <QFileDialog>
Dialog2  *m_dialog2;

Dialog1::Dialog1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog1)
{
    ui->setupUi(this);
    m_dialog2 = new Dialog2;
}

Dialog1::~Dialog1()
{
    delete ui;
}

void Dialog1::on_checkBox_clicked()
{
    human=false;

    this->hide();
    m_dialog2->show();
    //((MainWindow*)parent())->mdialog2->show();

    ui->checkBox->setCheckable(false);
}

/*
QMessageBox msgBox;
msgBox.setText("Test");
msgBox.exec();
*/


void Dialog1::on_pushButton_clicked()
{
    if(ui->radioButton->isChecked())
       {

        movingAlittle = true;
        movingAlot = false;


    }
    else if(ui->radioButton_3->isChecked())
    { movingAlot = true;
        movingAlittle = false;
        }
    // 파일 열어서 동영상 캡쳐하는 함수 넣기
    //main_cap();
    // 파일 열어서 동영상 캡쳐하는 함수 넣기
     QString dir = QFileDialog::getOpenFileName(this,"파일선택", "/Users/choeyunseo/","Files(*.*)");
     String dirStr = dir.toStdString();
     main_cap(dirStr);
      this->close();

}

