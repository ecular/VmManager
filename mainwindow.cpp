#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newvm.h"
#include "xmloperator.h"
#include <QDebug>
#include <QMessageBox>
#include <QTreeWidget>
#include "virshcmd.h"
#include "getvirtinfo.h"
#include "timestruct.h"
#include "settingdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*disable all action*/
    this->ui->action_Shutdown->setEnabled(false);
    this->ui->action_Suspend->setEnabled(false);
    this->ui->action_Kill->setEnabled(false);
    this->ui->action_Setting->setEnabled(false);
    this->ui->action_Delete_Vm->setEnabled(false);
    this->ui->action_Start->setEnabled(false);

    this->ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    //this->ui->treeWidget->header();

    FreshList();
    ListTimer = new QTimer(this);
    connect(ListTimer, SIGNAL(timeout()), this, SLOT(FreshList()));
    connect(this->ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRightMenu(void)));
    ListTimer->start(2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FreshList()
{
    //static bool first = true;
    if(!this->ui->treeWidget->currentItem())
        Selected_VM_name = "";
    else
        Selected_VM_name =  this->ui->treeWidget->currentItem()->text(1);
    this->ui->treeWidget->clear();
    int status = -1;
    XMLOperator xml;
    virshCMD virsh;
    timearg return_arg;
    GetVirtInfo getinfo;
    long long cpu_diff;
    long long real_diff;
    float usage = 0.0;
    QString VcpuCount;
    bool ok;
    vector<QString> VMname_vector = xml.GetVMnameVector();
    vector<QString>::iterator it = VMname_vector.begin();
    while(it < VMname_vector.end())
    {
        status = virsh.getVMstatus(*it);
        VcpuCount = xml.GetElement(*it, "vcpu");
        if(status != -1)
        {
            return_arg = getinfo.GetCpuTime(*it);
            if(VM_CPU_time.count(*it) == 1)
            {
                cpu_diff = (return_arg.info.cpuTime - VM_CPU_time[*it].info.cpuTime) / 1000;
                real_diff = 1000000 * (return_arg.real_time.tv_sec - VM_CPU_time[*it].real_time.tv_sec) + (return_arg.real_time.tv_usec - VM_CPU_time[*it].real_time.tv_usec);
                usage = cpu_diff / (float)(real_diff) * 100 / VcpuCount.toInt(&ok, 10);
                qDebug() << VcpuCount.toInt(&ok, 10);
                if(usage >= 100)
                    usage = 100.0;
                if(usage <= 0.0)
                    usage = 0.0;
            }
            VM_CPU_time[*it] = return_arg;
            VM_status[*it] = status;
            if(Selected_VM_name != *it)
                addVmToList(*it, usage, false);
            else
                addVmToList(*it, usage, true);
        }
        it++;
    }
    if(this->ui->treeWidget->currentItem())
        ReFreshEnable(this->ui->treeWidget->currentItem()->text(1));
}


void MainWindow::on_action_New_VM_triggered()
{
    ListTimer->stop();
    NewVM *newvmdlg = new NewVM(this);
    connect(newvmdlg, SIGNAL(addVM(void)), this, SLOT(FreshList(void)));
    newvmdlg->exec();
    ListTimer->start(2000);
}

void MainWindow::on_action_Exit_triggered()
{
    this->close();
}

void MainWindow::addVmToList(QString vm_name, float usage, bool selected)
{
    QStringList lisiChinese;
    QString status;
    if(VM_status[vm_name] == 0)
        status = "Stop";
    else if(VM_status[vm_name] == 1)
        status = "Running";
    else if(VM_status[vm_name] == 2)
        status = "Suspend";
    //qDebug() << VM_status[vm_name] << status << VM_cpusage[vm_name];
    int usage_int = (int)usage;
    lisiChinese << status << vm_name << QString("%1 %").arg(usage_int);
    QTreeWidgetItem *leaf2 = new QTreeWidgetItem(this->ui->treeWidget, lisiChinese);
    if(VM_status[vm_name] == 0)
        leaf2->setIcon(0, QIcon(":/icon/images/stop_status.png"));
    else if(VM_status[vm_name] == 1)
        leaf2->setIcon(0, QIcon(":/icon/images/run_status.png"));
    else if(VM_status[vm_name] == 2)
        leaf2->setIcon(0, QIcon(":/icon/images/suspend_status.png"));
    for(int i = 1; i < leaf2->columnCount(); i++)
    {
        leaf2->setTextAlignment(i, Qt::AlignHCenter);
    }

    this->ui->treeWidget->addTopLevelItem(leaf2);
    if(selected)
        this->ui->treeWidget->setCurrentItem(leaf2);
}

void MainWindow::on_action_Start_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    if(VM_status[VM_name] == 0)
    {
        XMLOperator xml;
        QString root_path = xml.GetElement(VM_name, "RootImgPath");
        QString private_path = xml.GetElement(VM_name, "PrivateImgPath");
        QString VMXmlLocation = xml.GetElement(VM_name, "XmlLocation");
        QString color_string = xml.GetElement(VM_name, "PrivateLevel");
        qDebug() << color_string;
        if(color_string == "Red")
            color_string = "0xFF0000";
        else if(color_string == "Green")
            color_string = "0x00FF00";
        else if(color_string == "Yellow")
            color_string = "0xFFFF00";
        qDebug() << color_string;
        /*create root.img*/
        QFile file(root_path);
        if(!file.exists())
            system(qPrintable("qemu-img create -f qcow2 -b /home/ecular/vm/vm2_root.img " + root_path));
        system(qPrintable("virsh undefine " + VM_name));
        system(qPrintable("virsh define " + VMXmlLocation));
        system(qPrintable("virsh start " + VM_name));
        sleep(1);
        system(qPrintable("server-side  " + color_string + " " + root_path + " " + private_path + " &"));
    }
    if(VM_status[VM_name] == 2)
    {
        system(qPrintable("virsh resume " + VM_name));
    }
    VM_status[VM_name] = 1;
}

void MainWindow::on_action_Delete_Vm_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    int ret1 = QMessageBox::question(this, tr("Delete VM"), tr(qPrintable("Delete VM " + VM_name + " ?")), QMessageBox::Yes, QMessageBox::No);
    if(ret1 == QMessageBox::Yes)
    {
        XMLOperator xml;
        QString root_path = xml.GetElement(VM_name, "RootImgPath");
        QString VMXmlLocation = xml.GetElement(VM_name, "XmlLocation");
        QString private_path = xml.GetElement(VM_name, "PrivateImgPath");
        QFile::remove(root_path);
        QFile::remove(VMXmlLocation);
        QFile::remove(private_path);
        if(!xml.DelNode(VM_name))
            QMessageBox::information(this, tr("Delete VM"), tr(qPrintable("Delete VM " + VM_name + " successfully!")), QMessageBox::Ok);
        map<QString , int >::iterator it;
        it = VM_status.find(VM_name);
        VM_status.erase(it);
        map<QString, timearg>::iterator it_time;
        it_time = VM_CPU_time.find(VM_name);
        VM_CPU_time.erase(it_time);
        FreshList();
        system(qPrintable("virsh undefine " + VM_name));
    }
}

void MainWindow::on_action_Suspend_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    system(qPrintable("virsh suspend " + VM_name));
    VM_status[VM_name] = 2;
    FreshList();
}

void MainWindow::on_action_Shutdown_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    system(qPrintable("virsh shutdown " + VM_name));
    VM_status[VM_name] = 0;
    FreshList();
}

void MainWindow::on_action_Kill_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    system(qPrintable("virsh destroy " + VM_name));
    VM_status[VM_name] = 0;
    FreshList();
}

void MainWindow::on_action_Setting_triggered()
{
    QString VM_name;
    if(this->ui->treeWidget->currentItem())
        VM_name = this->ui->treeWidget->currentItem()->text(1);
    else
        VM_name = Selected_VM_name;
    XMLOperator xml;
    QString Mac_tmp = xml.GetElement(VM_name, "Mac");
    ListTimer->stop();
    bool run = true;
    if(VM_status[VM_name] == 0)
        run = false;
    if(Mac_tmp != NULL)
    {
        SettingDialog Seting(this, VM_name, xml.GetElement(VM_name, "vcpu"), xml.GetElement(VM_name, "memory"), xml.GetElement(VM_name, "PrivateLevel"), true, Mac_tmp, run);
        Seting.exec();
    }
    else
    {
        SettingDialog Seting(this, VM_name, xml.GetElement(VM_name, "vcpu"), xml.GetElement(VM_name, "memory"), xml.GetElement(VM_name, "PrivateLevel"), false, "", run);
        Seting.exec();
    }
    ListTimer->start(2000);
}

void MainWindow::ReFreshEnable(QString VM_name)
{
    switch(VM_status[VM_name])/*0:stop 1:run 2:suspend*/
    {
    case(0):/*stop*/
    {
        /*enable:delete,run,setting*/
        this->ui->action_Delete_Vm->setEnabled(true);
        this->ui->action_Start->setEnabled(true);
        this->ui->action_Setting->setEnabled(true);

        /*disable:suspend,shutdown,kill*/
        this->ui->action_Suspend->setEnabled(false);
        this->ui->action_Shutdown->setEnabled(false);
        this->ui->action_Kill->setEnabled(false);
        break;
    }
    case(1):/*run*/
    {
        /*enable:shutdown,suspend,kill,setting*/
        this->ui->action_Shutdown->setEnabled(true);
        this->ui->action_Suspend->setEnabled(true);
        this->ui->action_Kill->setEnabled(true);
        this->ui->action_Setting->setEnabled(true);

        /*disable:run,delete*/
        this->ui->action_Delete_Vm->setEnabled(false);
        this->ui->action_Start->setEnabled(false);
        break;
    }
    case(2):/*suspend*/
    {
        /*enable:run,kill,setting*/
        this->ui->action_Kill->setEnabled(true);
        this->ui->action_Start->setEnabled(true);
        this->ui->action_Setting->setEnabled(true);

        /*disable:shutdown,delete,suspend*/
        this->ui->action_Delete_Vm->setEnabled(false);
        this->ui->action_Suspend->setEnabled(false);
        this->ui->action_Shutdown->setEnabled(false);
        break;
    }
    }
}

void MainWindow::showRightMenu(void)
{
    QCursor cur = this->cursor();
    QMenu *menu = new QMenu(this);
    menu->addAction(this->ui->action_New_VM);
    menu->addAction(this->ui->action_Delete_Vm);
    menu->addSeparator();
    menu->addAction(this->ui->action_Start);
    menu->addAction(this->ui->action_Suspend);
    menu->addAction(this->ui->action_Shutdown);
    menu->addAction(this->ui->action_Kill);
    menu->addSeparator();
    menu->addAction(this->ui->action_Setting);
    menu->exec(cur.pos()); //关联到光标
}

void MainWindow::on_treeWidget_itemSelectionChanged()
{
    if(this->ui->treeWidget->currentItem())
        ReFreshEnable(this->ui->treeWidget->currentItem()->text(1));
}
