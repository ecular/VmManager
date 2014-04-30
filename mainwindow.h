#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QTimer>
#include <timestruct.h>
#include <QTreeWidgetItem>

using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_New_VM_triggered();

    void on_action_Exit_triggered();

    void on_action_Start_triggered();

    void on_action_Delete_Vm_triggered();

    void on_action_Suspend_triggered();

    void on_action_Shutdown_triggered();

    void on_action_Kill_triggered();

    void FreshList();

    void on_action_Setting_triggered();

    void ReFreshEnable(QString VM_name);

    void showRightMenu(void);

    void on_treeWidget_itemSelectionChanged();

    void on_actionStart_Terminal_triggered();

    void on_actionStart_Firefox_triggered();

    void on_actionInput_triggered();

private:
    Ui::MainWindow *ui;
    QString Selected_VM_name;
    QTimer *ListTimer;
    map<QString, int> VM_status;/*0:stop 1:run 2:suspend*/
    map<QString, timearg> VM_CPU_time;
    void addVmToList(QString vm_name, float usage, bool selected);
};

#endif // MAINWINDOW_H
