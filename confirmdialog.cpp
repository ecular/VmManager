#include "confirmdialog.h"
#include "ui_confirmdialog.h"

ConfirmDialog::ConfirmDialog(QWidget *parent, QString arg) :
    QDialog(parent),
    ui(new Ui::ConfirmDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->appendPlainText("\n" + arg);
}

ConfirmDialog::~ConfirmDialog()
{
    delete ui;
}

void ConfirmDialog::on_buttonBox_accepted()
{
    accept();
}

void ConfirmDialog::on_buttonBox_rejected()
{
    reject();
}
