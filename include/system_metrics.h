#ifndef SYSTEM_METRICS_H
#define SYSTEM_METRICS_H

#include <time.h>

#define MAX_CORES 64
#define MAX_PROCESSES 1024
#define STRING_SHORT 32
#define STRING_MEDIUM 64

// Informations sur un processus individuel
typedef struct {
    int pid;
    char name[STRING_MEDIUM];
    char user[STRING_SHORT];
    char status[STRING_SHORT];   // ex: "Running", "Sleeping", "Zombie"
    float cpu_percent;
    float mem_percent;
    unsigned long mem_rss_kb;    // RAM résidente utilisée
} ProcessInfo;

// Snapshot complet de l'état système à un instant t
typedef struct {
    time_t timestamp;                     // Date/Heure de capture
    
    // --- CPU (Module Étudiant 1) ---
    float cpu_total_usage;                // % d'usage global (0.0 à 100.0)
    int cpu_core_count;                   // Nombre de cœurs détectés
    float cpu_cores_usage[MAX_CORES];     // % d'usage par cœur
    float cpu_frequency_mhz;              // Fréquence actuelle en MHz
    
    // --- Mémoire (Module Étudiant 1) ---
    unsigned long mem_total_kb;           // Mémoire totale
    unsigned long mem_used_kb;            // Mémoire utilisée
    unsigned long mem_free_kb;            // Mémoire libre
    unsigned long mem_cached_kb;          // Mémoire cache / buffers
    
    // --- Disque (Module Étudiant 2) ---
    char disk_mount_point[STRING_SHORT];  // ex: "/"
    unsigned long long disk_total_mb;     // Espace total
    unsigned long long disk_used_mb;      // Espace utilisé
    unsigned long long disk_free_mb;      // Espace libre
    
    // --- Réseau (Module Étudiant 2) ---
    char net_interface[STRING_SHORT];     // ex: "eth0", "wlan0"
    float net_rx_kbs;                     // Débit descendant (Download en Ko/s)
    float net_tx_kbs;                     // Débit montant (Upload en Ko/s)
    
    // --- Processus (Module Étudiant 2) ---
    ProcessInfo processes[MAX_PROCESSES];
    int process_count;
} SystemSnapshot;

#endif // SYSTEM_METRICS_H
