#ifndef VIRSHCMD_H
#define VIRSHCMD_H
#include <QString>
using namespace std;

class virshCMD
{
public:
    virshCMD();
private:
    int mysystem(const char *cmdstring, char* buf, int len);
public:
    int getVMstatus(QString VM_name);
};

#endif // VIRSHCMD_H
