#include <string>
#include <QFileDialog>
#include <QDebug>
#include <QProgressDialog>
#include "newvm.h"
#include "ui_newvm.h"
#include "confirmdialog.h"
#include "xmloperator.h"

NewVM::NewVM(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewVM)
{
    ui->setupUi(this);
    XMLOperator xml;
    VMname_vector = xml.GetVMnameVector();
    vector<QString>::iterator it = VMname_vector.begin();

    bool ok;
    int mac_int = 0;
    QString Mac_tmp;
    while(it < VMname_vector.end())
    {
        Mac_tmp = xml.GetElement(*it, "Mac");
        if(Mac_tmp != NULL)
        {
            Mac_vector.push_back(Mac_tmp);
            int tmp_mac_int = Mac_tmp.mid(Mac_tmp.lastIndexOf(":", -1) + 1).toInt(&ok, 16);
            if(mac_int < tmp_mac_int)
                mac_int = tmp_mac_int;
            //qDebug() << "name:" << *it << "Mac:" << Mac_tmp << " " << mac_int;
        }
        it++;
    }
    NewMac = /*Mac_tmp.mid(0, Mac_tmp.lastIndexOf(":", -1) + 1)*/"52:54:00:12:34:" + QString::number(++mac_int, 16);
    //qDebug() << "NewMac:" << NewMac;

    ui->lineEdit_2->setText(NewMac);
}

NewVM::~NewVM()
{
    delete ui;
}

void NewVM::on_pushButton_clicked()
{
    QString arg = "VM name:\t" + ui->lineEdit->text() + "\nVCPU Count:\t" + ui->comboBox_2->currentText() + "\nMemory Size:\t" + ui->spinBox_2->text() + " Mb\nPrivate Level:\t" + ui->comboBox->currentText() + "\n";
    ConfirmDialog confirmdlg(this, arg);
    if(confirmdlg.exec() == QDialog::Accepted)
    {
        XMLOperator xml;
        bool ok;

        xml.AddNode("./test.xml", ui->lineEdit->text(), ui->comboBox_2->currentText(), QString::number(ui->spinBox_2->text().toInt(&ok, 10) * 1024, 10), ui->comboBox->currentText(), ui->lineEdit_3->text(), ui->lineEdit_3->text().mid(0, ui->lineEdit_3->text().lastIndexOf("/", -1) + 1) + ui->lineEdit->text() + "_root.img", ui->lineEdit_2->text(), ui->lineEdit->text() + ".xml");
        xml.CreateVMXml(ui->lineEdit->text() + ".xml", ui->lineEdit->text(), ui->comboBox_2->currentText(), QString::number(ui->spinBox_2->text().toInt(&ok, 10) * 1024, 10), ui->lineEdit_3->text(), ui->lineEdit_3->text().mid(0, ui->lineEdit_3->text().lastIndexOf("/", -1) + 1) + ui->lineEdit->text() + "_root.img", ui->lineEdit_2->text());

        /*create root.*/
        //system(qPrintable("qemu-img create -f qcow2 -b /home/ecular/vm/vm1_root.img " + ui->lineEdit_3->text().mid(0, ui->lineEdit_3->text().lastIndexOf("/", -1) + 1) + ui->lineEdit->text() + "_root.img"));

        /*copy private.img file*/
        int readlen;
        char databuff[4096];
        QFile file_read("/home/ecular/vm/vm2_private.img");
        file_read.open(QIODevice::ReadOnly);
        QFile file_write(ui->lineEdit_3->text());
        file_write.open(QIODevice::WriteOnly);
        QProgressDialog dialog("Creating VM " + ui->lineEdit->text() + ", Please wait ...", tr("Cancel"), 0, file_read.size(), this);
        dialog.setWindowTitle(tr("Creating"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        while(!file_read.atEnd())
        {
            readlen = file_read.read(databuff, 4096);
            file_write.write(databuff, readlen);
            dialog.setValue(file_read.pos());
            QCoreApplication::processEvents();
            if(dialog.wasCanceled())
            {
                /*delete rubish*/
                xml.DelNode(ui->lineEdit->text());
                QFile::remove(ui->lineEdit->text() + ".xml");
                QFile::remove(ui->lineEdit_3->text());
                //QFile::remove("/tmp/vm1_root.img");
                return;
            }
        }
        file_read.close();
        file_write.close();
        system(qPrintable("virsh define " + ui->lineEdit->text() + ".xml"));
        emit addVM();
        this->close();
    }

}

void NewVM::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked())
    {
        ui->label_7->setEnabled(true);
        ui->lineEdit_2->setText(NewMac);
        ui->lineEdit_2->setEnabled(true);
    }
    else
    {
        ui->label_7->setEnabled(false);
        ui->lineEdit_2->setText("");
        ui->lineEdit_2->setEnabled(false);
    }

}

void NewVM::on_pushButton_3_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Directory Dialog"), "/var/", QFileDialog::ShowDirsOnly);
    ui->lineEdit_3->setText(dirName + "/" + ui->lineEdit->text() + ".img");
}

void NewVM::on_lineEdit_editingFinished()
{
    if(ui->lineEdit->text() != NULL)
        ui->lineEdit_3->setText(ui->lineEdit_3->text().mid(0, ui->lineEdit_3->text().lastIndexOf("/", -1) + 1) + ui->lineEdit->text() + ".img");
}
