#ifndef XMLOPERATOR_H
#define XMLOPERATOR_H

#include <QString>
#include <QFile>
#include <QtXml/QDomDocument>
#include <vector>

using namespace std;

class XMLOperator
{

public:
    XMLOperator();

    int AddNode(QString file_location, QString vm_name, QString vcpu, QString memory, QString privateLevel, QString PrivateimgPath, QString rootimgPath, QString macAddress, QString xmlLocation);

    int DelNode(QString vm_name);

    QString GetElement(QString vm_name, QString element);

    int CreateVMXml(QString file_location, QString vm_name, QString vcpu, QString memory, QString PrivateimgPath, QString RootimgPath, QString macAddress);

    vector<QString> GetVMnameVector();
};

#endif // XMLOPERATOR_H
