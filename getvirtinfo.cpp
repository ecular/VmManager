#include "getvirtinfo.h"
#include <QDebug>

GetVirtInfo::GetVirtInfo()
{
    conn = virConnectOpen("qemu:///system");
    if(conn == NULL) {
        qDebug() << "error connecting qemu driver\n";
        exit(1);
    }
}

GetVirtInfo::~GetVirtInfo()
{
    virConnectClose(conn);
}

timearg GetVirtInfo::GetCpuTime(QString VM_name)
{
    virDomainPtr vm_ptr = virDomainLookupByName(conn, qPrintable(VM_name));
    if(vm_ptr == NULL) {
        printf("error finding domain\n");
        virConnectClose(conn);
        exit(1);
    }
    virDomainInfo info_s;
    struct timeval real_time_s;
    timearg return_arg;

    if(virDomainGetInfo(vm_ptr, &info_s) != 0) {
        printf("error get domain info\n");
        virDomainFree(vm_ptr);
        virConnectClose(conn);
        exit(1);
    }

    if(gettimeofday(&real_time_s, NULL) == -1) {
        printf("error get time of day\n");
        virDomainFree(vm_ptr);
        virConnectClose(conn);
        exit(1);
    }
    return_arg.info = info_s;
    return_arg.real_time = real_time_s;
    virDomainFree(vm_ptr);
    return return_arg;

    /* sleep(1);

     if(virDomainGetInfo(vm_ptr, &info_e) !=0) {
         printf("error get domain info\n");
         virDomainFree(vm_ptr);
         virConnectClose(conn);
         exit(1);
     }

     if(gettimeofday(&real_time_e, NULL) == -1) {
         printf("error get time of day\n");
         virDomainFree(vm_ptr);
         virConnectClose(conn);
         exit(1);
     }
     cpu_diff = (info_e.cpuTime - info_s.cpuTime) / 1000;
     real_diff = 1000000 * (real_time_e.tv_sec - real_time_s.tv_sec) +(real_time_e.tv_usec - real_time_s.tv_usec);
     virDomainFree(vm_ptr);
     usage = cpu_diff / (float) (real_diff);
     return usage*100;*/
}
