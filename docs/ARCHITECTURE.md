# Spécifications et Guide d'Architecture - Moniteur de Ressources Linux (C / GUI)

## 1. Vision et Objectifs du Projet
Développer en C sous Linux un moniteur de ressources système avec une interface visuelle graphique (GUI), capable de :
1. Mesurer et afficher en direct les métriques matérielles (CPU, RAM, Disque, Réseau) et la liste des processus.
2. Prendre un **instantané (snapshot)** complet de l'état du système à l'instant *t* et le sauvegarder dans un fichier **CSV**.
3. Recharger et visualiser un snapshot CSV préalablement enregistré.

---

## 2. Répartition Équilibrée (Full-Stack : Back + Front pour les 2 étudiants)

Puisque chacun doit toucher à la fois au **système (C Linux / `/proc`)** et à l'**interface graphique (GUI)**, le découpage optimal se fait par **modules fonctionnels verticaux** (Feature Teams) :

```
                  ┌───────────────────────────────────────────────┐
                  │                 SOCLE COMMUN                  │
                  │  Fenêtre principale, boucle d'événements,     │
                  │  structures globales (system_metrics.h) &     │
                  │  Makefile                                     │
                  └───────────────────────┬───────────────────────┘
                                          │
            ┌─────────────────────────────┴─────────────────────────────┐
            ▼                                                           ▼
┌───────────────────────────────────────┐   ┌───────────────────────────────────────┐
│        ÉTUDIANT 1 : VITAL STATS       │   │        ÉTUDIANT 2 : SYSTEM & IO       │
│                                       │   │                                       │
│ 1. Backend : CPU & RAM                │   │ 1. Backend : Disque, Réseau & Proc    │
│    - /proc/stat, /proc/meminfo        │   │    - statvfs, /proc/net/dev, /proc/PID│
│ 2. Frontend : Jauges CPU & RAM        │   │ 2. Frontend : Vues Réseau & Table Proc│
│ 3. Données : Export Snapshot CSV      │   │ 3. Données : Import/Parsing CSV       │
└───────────────────────────────────────┘   └───────────────────────────────────────┘
```

| Domaine | Étudiant 1 | Étudiant 2 |
| :--- | :--- | :--- |
| **Backend (Système)** | **CPU & RAM**<br>• Calcul charge globale & par cœur<br>• Fréquence CPU<br>• RAM utilisée, libre, cache | **Disque, Réseau & Processus**<br>• Espace partitions (`statvfs`)<br>• Débits RX/TX (KB/s)<br>• Scan `/proc` pour lister les processus |
| **Frontend (GUI)** | **Vues CPU & Mémoire**<br>• Jauges circulaires ou barres de charge<br>• Graphique historique ou barres par cœur<br>• Bouton "Sauvegarder Snapshot" | **Vues I/O & Processus**<br>• Indicateurs Réseau (Upload/Download)<br>• Jauge stockage partition<br>• Tableau déroulant de la liste des processus<br>• Bouton "Ouvrir / Revoir Snapshot" |
| **Persistance CSV** | **Exportation** (`snapshot_export_csv()`) : Écriture structurée de toutes les métriques dans un fichier CSV. | **Importation** (`snapshot_import_csv()`) : Lecture, parsing du fichier CSV et restitution dans la structure pour réaffichage. |

---

## 3. Arborescence Recommandée du Code

```text
Projet-c/
├── Makefile                     # Compilation avec flags (-Wall -Wextra, libs GUI)
├── README.md                    # Présentation et instructions de lancement
├── docs/
│   └── ARCHITECTURE.md          # Ce document
├── include/
│   ├── system_metrics.h         # Fichier pivot : structures de données partagées
│   ├── cpu_mem.h                # Prototypes Étudiant 1 (CPU + RAM)
│   ├── disk_net_proc.h          # Prototypes Étudiant 2 (Disque + Réseau + Processus)
│   ├── snapshot.h               # Prototypes Export (Étudiant 1) & Import (Étudiant 2)
│   └── gui.h                    # Prototypes de l'interface graphique (fenêtre, panels)
└── src/
    ├── main.c                   # Initialisation, boucle d'affichage et timer de rafraîchissement
    ├── cpu_mem.c                # Backend CPU/RAM (/proc/stat, /proc/meminfo)
    ├── disk_net_proc.c          # Backend Disque, Réseau, Processus
    ├── snapshot.c               # Fonctions d'écriture et de lecture CSV
    └── gui.c                    # Implémentation graphique (panneaux, jauges, tableau)
```

---

## 4. Spécifications Détaillées des Fonctionnalités & Pistes Techniques

### Module 1 : CPU (Étudiant 1)
* **Charge CPU Globale & Par Cœur** :
  * *Où chercher :* `/proc/stat`.
  * *Formule & Piste :* Les valeurs de `/proc/stat` sont des **compteurs cumulés** de ticks depuis le démarrage (`user, nice, system, idle, iowait, irq, softirq`).
  * *Piège classique :* On ne peut pas calculer un pourcentage avec une seule lecture ! Il faut conserver l'état de la mesure précédente $t_0$, lire à $t_1$, calculer $\Delta \text{total}$ et $\Delta \text{idle}$ :
    $$\text{Usage} = \frac{\Delta \text{total} - \Delta \text{idle}}{\Delta \text{total}} \times 100$$
  * *Par cœur :* La première ligne `cpu` donne la moyenne globale. Les lignes suivantes `cpu0`, `cpu1`, etc. donnent le détail cœur par cœur.
* **Fréquence CPU** :
  * *Où chercher :* `/proc/cpuinfo` (chercher la ligne `cpu MHz`) ou `/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` (diviser par 1000 pour obtenir des MHz).

### Module 2 : Mémoire RAM (Étudiant 1)
* **RAM Totale, Utilisée, Libre, Cache** :
  * *Où chercher :* `/proc/meminfo`.
  * *Champs clés :*
    * `MemTotal`
    * `MemFree`
    * `Buffers`
    * `Cached`
    * `MemAvailable`
  * *Calcul recommandé sous Linux :*
    $$\text{RAM Utilisée} = \text{MemTotal} - \text{MemFree} - \text{Buffers} - \text{Cached}$$
    (Ou simplement $\text{MemTotal} - \text{MemAvailable}$).

---

### Module 3 : Disque & Stockage (Étudiant 2)
* **Espace Partition (Racine `/` ou `/home`)** :
  * *Où chercher :* Utiliser l'appel système standard POSIX `statvfs()` (`#include <sys/statvfs.h>`).
  * *Piste & Calcul :*
    ```c
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long long total = stat.f_blocks * stat.f_frsize;
        unsigned long long free  = stat.f_bfree  * stat.f_frsize;
        unsigned long long used  = total - free;
    }
    ```

### Module 4 : Réseau (Étudiant 2)
* **Débits Montant (TX) et Descendant (RX)** :
  * *Où chercher :* `/proc/net/dev`.
  * *Piste :* Repérer l'interface active (ex: `eth0`, `wlan0`, ou ignorer `lo`).
  * *Champs :* Colonne 1 (`bytes` reçus) et Colonne 9 (`bytes` transmis).
  * *Calcul :* Même logique que le CPU : faire la différence entre deux lectures espacées de $\Delta t$ :
    $$\text{Débit (Ko/s)} = \frac{\text{Bytes}_{t_1} - \text{Bytes}_{t_0}}{1024 \times \Delta t}$$

### Module 5 : Liste des Processus (Étudiant 2)
* **Scan des Processus Actifs** :
  * *Où chercher :* Parcourir le dossier `/proc/` avec `opendir("/proc")` et `readdir()`.
  * *Piste :* Ne garder que les sous-dossiers dont le nom est entièrement numérique (ce sont les PIDs, ex: `/proc/1234/`).
  * *Pour chaque PID :*
    * **Nom & Statut :** Lire `/proc/[PID]/status` (champs `Name:`, `State:`).
    * **Utilisateur (UID) :** Champ `Uid:` dans `/proc/[PID]/status`, puis convertir avec `getpwuid()`.
    * **Mémoire :** Champ `VmRSS:` dans `/proc/[PID]/status` (taille résidente en RAM).
    * **CPU :** Champ `utime` + `stime` dans `/proc/[PID]/stat`.

---

### Module 6 : Sauvegarde & Relecture Snapshot CSV (Étudiant 1 & 2)
* **Structure du CSV** :
  Le fichier CSV peut contenir 2 sections ou un format prédictible :
  1. **L'en-tête Métriques Système :**
     `TIMESTAMP,CPU_GLOBAL,MEM_TOTAL,MEM_USED,DISK_TOTAL,DISK_USED,NET_RX,NET_TX`
  2. **La section Processus :**
     `PID,NAME,USER,CPU_PERCENT,MEM_KB,STATUS`
* **Étudiant 1 (Export) :**
  * Ouvre le fichier en écriture (`fopen("snapshot_20261111_2030.csv", "w")`).
  * Écrit la ligne des métriques globales, puis itère sur la liste des processus.
* **Étudiant 2 (Import & Relecture) :**
  * Ouvre le fichier sélectionné avec `fgets()` et `sscanf()` ou `strtok()`.
  * Remplie la structure `SystemSnapshot`.
  * Passe l'interface en mode **"Mode Archive : Visualisation d'un Snapshot"** (les jauges affichent les données du fichier au lieu du direct).

---

## 5. Gestion de la GUI sans Multi-Threading

Sans multi-threading, comment animer la fenêtre tout en actualisant les données ?
* **Principe de la boucle événementielle avec Timer :**
  Toutes les bibliothèques GUI (GTK, Raylib, SDL2) proposent un mécanisme de rafraîchissement périodique :
  * En **GTK** : `g_timeout_add_seconds(1, on_timer_tick, user_data);`
  * En **Raylib / SDL2** : Dans la boucle `while (!should_close)`, on mesure le temps écoulé avec une horloge (`GetTime()` ou `SDL_GetTicks()`). Dès que 1 seconde s'est écoulée, on appelle la fonction de collecte des données système, puis on redessine l'écran.

---

## 6. Prochaines Étapes Immédiates

1. **Valider ensemble le fichier d'en-tête commun** `include/system_metrics.h`.
2. **Créer le `Makefile`** de base.
3. Chaque étudiant peut commencer son module avec un petit `main_test.c` en ligne de commande pour vérifier les données avant même d'assembler la partie graphique !
