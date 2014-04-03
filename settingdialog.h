#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <vector>
#include <QString>
#include <string>

using namespace std;

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent, QString domain_name, QString VCPU, QString Memory, QString Private_level, bool connect_internet, QString mac, bool run);
    ~SettingDialog();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::SettingDialog *ui;
    QString ori_VM_name;
    bool connect_internet_flag;
    QString MAC;
    QString NewMac;
};

#endif // SETTINGDIALOG_H
