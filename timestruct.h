#ifndef TIMESTRUCT_H
#define TIMESTRUCT_H
#include <sys/time.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

typedef struct
{
    virDomainInfo info;
    struct timeval real_time;
} timearg;
#endif // TIMESTRUCT_H
