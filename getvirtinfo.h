#ifndef GETVIRTINFO_H
#define GETVIRTINFO_H
#include <QString>
#include <sys/time.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include "timestruct.h"

class GetVirtInfo
{
public:
    GetVirtInfo();
    ~GetVirtInfo();
    timearg GetCpuTime(QString VM_name);
private:
    virConnectPtr conn;
};

#endif // GETVIRTINFO_H
