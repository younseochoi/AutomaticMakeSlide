#include "dialog2.h"
#include "ui_dialog2.h"
#include "code.h"
#include <QFileDialog>


Dialog2::Dialog2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog2)
{
    ui->setupUi(this);
}

Dialog2::~Dialog2()
{
    delete ui;
}

void Dialog2::on_pushButton_clicked()
{
    if(ui->checkBox_2->isChecked())   // 쪽수있을시
        page_num = true;
    if(ui->checkBox->isChecked())    // 제목있을시
        title=true;

    // 파일 열어서 동영상 캡쳐하는 함수 넣기
     QString dir = QFileDialog::getOpenFileName(this,"파일선택", "/Users/choeyunseo/","Files(*.*)");
     String dirStr = dir.toStdString();
     main_cap(dirStr);
     this->close();

}

