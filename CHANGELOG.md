# Historique des modifications - Projet 42

### 2025-02-25 - srcs/garbage_collector.c
- [BUGFIX] Correction des problèmes de double free dans le garbage collector
  ► Impact : Meilleure stabilité et moins de fuites mémoire
  ► Modification : Séparation claire entre la gestion des nœuds GC et des données utilisateur

- [CODE] Correction des erreurs de syntaxe dans le fichier garbage_collector.c
  ► Impact : Meilleure conformité à la norme et compilation sans erreurs
  ► Modifications : Nettoyage des commentaires, respect des règles de style, correction des points-virgules manquants

- [OPTI] Amélioration de la fonction ft_gc_remove_list pour éviter les double frees
  ► Ancienne implémentation : Libérait les pointeurs dans un ordre qui causait des accès à de la mémoire déjà libérée
  ► Nouvelle implémentation : Sépare le retrait des nœuds du GC et la libération de la mémoire pour plus de sûreté