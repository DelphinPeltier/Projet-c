#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/cpu.h"
#include "../include/system_metrics.h"

typedef struct {
    unsigned long temps_total;
    unsigned long temps_inactif;
} cpu_stats;

cpu_stats get_cpu_temp(void){
    FILE *f = fopen("/proc/stat", "r");
    if (f == NULL){
        printf("echec de l'ouverture du fichier");
        return (cpu_stats){0,0};
    }
    char ligne[256];
    int valeur[10];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (sscanf(ligne, "cpu %d %d %d %d %d %d %d %d %d %d", &valeur[0], &valeur[1], &valeur[2], &valeur[3], &valeur[4], &valeur[5], &valeur[6], &valeur[7], &valeur[8], &valeur[9]) == 10) {
                unsigned long temps_inactif = valeur[3]+valeur[4];
                unsigned long temps_total = valeur[0] + valeur[1] + valeur[2] + valeur[3] + valeur[4] + valeur[5] + valeur[6]+valeur[7]+valeur[8]+valeur[9];
                return (cpu_stats){.temps_total = temps_total, .temps_inactif = temps_inactif} ;
        }
    }
    fclose(f);
    return (cpu_stats){0,0};
}

int get_cpu_info(SystemSnapshot *snap){
    //calcul du taux de cpu sur 1s
    cpu_stats temps1 = get_cpu_temp();
    sleep(1);
    cpu_stats temps2 = get_cpu_temp();
    unsigned long delta_total = temps2.temps_total - temps1.temps_total;
    unsigned long delta_inactif = temps2.temps_inactif - temps1.temps_inactif;
    snap->cpu_total_usage = ((float)(delta_total - delta_inactif) / delta_total) * 100;

    //NB de coeurs et cpu frequence
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f == NULL){
        printf("echec de l'ouverture du fichier");
        return 1;
    }
    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (strncmp(ligne, "siblings", 8) == 0) {
            char *p = strchr(ligne, ':');
            if (p) sscanf(p + 1, "%d", &snap->cpu_core_count);
        }
        else if (strncmp(ligne, "cpu MHz", 7) == 0) {
            char *p = strchr(ligne, ':');
            if (p) sscanf(p + 1, "%f", &snap->cpu_frequency_mhz);
        }
    }
    fclose(f);
    return 0;
}