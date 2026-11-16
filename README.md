# Moniteur de Ressources Système sous Linux (C / GUI)

Projet scolaire réalisé en langage C visant à concevoir un moniteur de ressources système pour Linux avec une interface graphique visuelle et un système de capture d'instantanés (snapshots).

---

## 📌 Présentation du Projet

Cette application permet de surveiller en temps réel l'état matériel et logiciel de la machine Linux, tout en offrant la possibilité de figer l'état du système pour une analyse ultérieure.

---

## 🛠️ Compilation et Lancement

*(À venir avec le Makefile)*
```bash
# Compilation
make

# Exécution
./bin/sysmonitor
```


### Fonctionnalités principales :

1. **Surveillance en temps réel (Mode Live) :**
   * **Processeur (CPU) :** Charge globale, pourcentage d'utilisation par cœur et fréquence actuelle (MHz).
   * **Mémoire (RAM) :** Suivi de la mémoire totale, utilisée, libre et mise en cache.
   * **Stockage (Disque) :** Espace total, occupé et restant sur la partition principale.
   * **Réseau :** Débits entrants (Download) et sortants (Upload) en Ko/s.
   * **Processus :** Liste des processus en cours d'exécution (PID, nom, utilisateur, consommation CPU/RAM, statut).

2. **Instantanés & Historique (Mode Archive / CSV) :**
   * **Export Snapshot :** Enregistre l'ensemble des métriques et processus à l'instant *t* dans un fichier `.csv`.
   * **Relecture Snapshot :** Permet de charger un fichier CSV pour réafficher visuellement dans l'interface l'état exact du système au moment de la capture.


   biliothéque graphique: lvg
---

## 👥 Répartition du Binôme

Le projet est conçu de manière modulaire, associant développement système (Linux `/proc`) et interface graphique (GUI) pour chaque membre :

* **Étudiant 1 :** Module Vital Stats (CPU, RAM) & Exportation CSV.
* **Étudiant 2 :** Module I/O & Processus (Disque, Réseau, Processus) & Importation/Relecture CSV.

Pour plus de détails techniques sur les formules et l'architecture du code, consultez la documentation dans [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

