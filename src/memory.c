#include <stdio.h>
#include <string.h>
#include "../include/memory.h"
#include "../include/system_metrics.h"

int get_memory_info(SystemSnapshot *snap){
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL){
        printf("echec de l'ouverture du fichier");
        return 1;
    }
    char ligne[256];
    char nom[100];
    int valeur;

    int Memtotal = 0;
    int MemAvailable = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (sscanf(ligne, " %99[^:] : %d", nom, &valeur) == 2) {
            if (strcmp(nom, "MemTotal") == 0) {
                Memtotal = valeur;
            } else if (strcmp(nom, "MemAvailable") == 0) {
                MemAvailable = valeur;
            }
        }
    }

    fclose(f);

    if(Memtotal == 0){
        snap->mem_used_pourcentage = 0;
        snap->mem_used_kb = 0;
        snap->mem_free_kb = 0;
        snap->mem_total_kb = 0;
    }else{
        snap->mem_used_pourcentage = (double) ((double)(Memtotal - MemAvailable) / (double)Memtotal) * 100;
        snap->mem_used_kb = (unsigned long) (Memtotal - MemAvailable);
        snap->mem_free_kb = (unsigned long) MemAvailable;
        snap->mem_total_kb = (unsigned long) Memtotal;
    }

    return 0;
}