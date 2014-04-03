#ifndef NEWVM_H
#define NEWVM_H

#include <QDialog>
#include <vector>
#include <QString>
#include <string>

using namespace std;

namespace Ui {
class NewVM;
}

class NewVM : public QDialog
{
    Q_OBJECT

signals:
    void addVM();

public:
    explicit NewVM(QWidget *parent = 0);
    ~NewVM();

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_3_clicked();

    //void on_lineEdit_textEdited(const QString &arg1);

    //void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_editingFinished();

private:
    Ui::NewVM *ui;
    vector<QString> VMname_vector;
    vector<QString> Mac_vector;
    QString NewMac;
};

#endif // NEWVM_H
