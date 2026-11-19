#ifndef DISK_H
#define DISK_H
#include "system_metrics.h"

int disk_get(DiskInfo *d);   /* 0 si OK, -1 si erreur */

#endif