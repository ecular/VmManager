#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "xmloperator.h"

SettingDialog::SettingDialog(QWidget *parent, QString domain_name, QString VCPU, QString Memory, QString Private_level, bool connect_internet, QString mac, bool run):
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    if(run == true)
    {
        this->ui->pushButton->setEnabled(false);
        this->ui->lineEdit->setEnabled(false);
        this->ui->comboBox->setEnabled(false);
        this->ui->spinBox_2->setEnabled(false);
        this->ui->horizontalSlider_2->setEnabled(false);
        this->ui->comboBox_2->setEnabled(false);
    }
    ori_VM_name = domain_name;
    bool ok;
    connect_internet_flag = connect_internet;
    MAC = mac;
    this->ui->lineEdit->setText(domain_name);
    this->ui->comboBox->setCurrentText(VCPU);
    this->ui->spinBox_2->setValue(Memory.toInt(&ok, 10) / 1024);
    this->ui->comboBox_2->setCurrentText(Private_level);
    if(connect_internet)
    {
        this->ui->checkBox->setChecked(true);
        this->ui->label_6->setEnabled(true);
        this->ui->lineEdit_2->setText(mac);
    }
    else
    {
        ui->label_6->setEnabled(false);
        ui->lineEdit_2->setText("");
        ui->lineEdit_2->setEnabled(false);
        XMLOperator xml;
        vector<QString> VMname_vector;
        vector<QString> Mac_vector;
        VMname_vector = xml.GetVMnameVector();
        vector<QString>::iterator it = VMname_vector.begin();
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
            }
            it++;
        }
        NewMac = /*Mac_tmp.mid(0, Mac_tmp.lastIndexOf(":", -1) + 1)*/"52:54:00:12:34:" + QString::number(++mac_int, 16);
    }
    if(run == true)
    {
        this->ui->checkBox->setEnabled(false);
        this->ui->label_6->setEnabled(false);
        this->ui->lineEdit_2->setEnabled(false);
    }
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_checkBox_stateChanged(int arg1)
{
    if(!connect_internet_flag)
    {
        if(ui->checkBox->isChecked())
        {
            ui->label_6->setEnabled(true);
            ui->lineEdit_2->setText(NewMac);
            ui->lineEdit_2->setEnabled(true);
        }
        else
        {
            ui->label_6->setEnabled(false);
            ui->lineEdit_2->setText("");
            ui->lineEdit_2->setEnabled(false);
        }
    }
    else
    {
        if(ui->checkBox->isChecked())
        {
            ui->label_6->setEnabled(true);
            ui->lineEdit_2->setText(MAC);
            ui->lineEdit_2->setEnabled(true);
        }
        else
        {
            ui->label_6->setEnabled(false);
            ui->lineEdit_2->setText("");
            ui->lineEdit_2->setEnabled(false);
        }
    }
}

void SettingDialog::on_pushButton_2_clicked()
{
    this->close();
}

void SettingDialog::on_pushButton_clicked()
{
    XMLOperator xml;
    bool ok;
    QString VMXmlLocation = xml.GetElement(ori_VM_name, "XmlLocation");
    system(qPrintable("virsh undefine " + ori_VM_name));
    /*delete vm xml*/
    QFile::remove(VMXmlLocation);
    QString PrivateImg = xml.GetElement(ori_VM_name, "PrivateImgPath");
    QString RootImg = xml.GetElement(ori_VM_name, "RootImgPath");
    QString XmlLocation = xml.GetElement(ori_VM_name, "XmlLocation");

    QString cmd_1 = "mv " + PrivateImg;
    QString cmd_2 = "mv " + RootImg;

    VMXmlLocation.replace(VMXmlLocation.indexOf(ori_VM_name), ori_VM_name.size(), this->ui->lineEdit->text());
    PrivateImg.replace(PrivateImg.indexOf(ori_VM_name), ori_VM_name.size(), this->ui->lineEdit->text());
    RootImg.replace(RootImg.indexOf(ori_VM_name), ori_VM_name.size(), this->ui->lineEdit->text());
    XmlLocation.replace(XmlLocation.indexOf(ori_VM_name), ori_VM_name.size(), this->ui->lineEdit->text());

    /*rename VM img*/
    cmd_1 = cmd_1 + " " + PrivateImg;
    cmd_2 = cmd_2 + " " + RootImg;
    system(qPrintable(cmd_1));
    system(qPrintable(cmd_2));

    /*delete item from test.xml*/
    xml.DelNode(ori_VM_name);

    /*add new Node into test.xml*/
    xml.AddNode("./test.xml", this->ui->lineEdit->text(), this->ui->comboBox->currentText(), QString::number(ui->spinBox_2->text().toInt(&ok, 10) * 1024, 10), this->ui->comboBox_2->currentText(), PrivateImg, RootImg, ui->lineEdit_2->text(), XmlLocation);
    /*create new vm xml*/
    xml.CreateVMXml(VMXmlLocation, this->ui->lineEdit->text(), this->ui->comboBox->currentText(), QString::number(ui->spinBox_2->text().toInt(&ok, 10) * 1024, 10), PrivateImg, RootImg, ui->lineEdit_2->text());
    system(qPrintable("virsh define " + ui->lineEdit->text() + ".xml"));
    this->close();
}
