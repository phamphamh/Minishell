# Historique des modifications - Projet 42

### 2025-02-25 - srcs/parser/tokenizer.c
- [BUGFIX] Correction des problèmes de double free dans la fonction ft_tokenize
  ► Impact : Résolution de l'erreur "Invalid free() / delete / delete[] / realloc()" signalée par Valgrind
  ► Modification : Suppression de l'ajout du pointeur expanded_input au garbage collector puisqu'il est libéré manuellement plus tard dans la fonction

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

### 2025-02-25 - srcs/utils.c

- **Correction :** Amélioration de la fonction `ft_clean_exit` pour nettoyer explicitement les tokens et les commandes avant de quitter, résolvant les problèmes de mémoire "still reachable" rapportés par Valgrind.
- **Impact :** Réduit considérablement la mémoire non libérée à la fin du programme.
- **Modification :** Ajout d'étapes explicites pour nettoyer les tokens et les commandes en appelant les fonctions spécifiques du garbage collector.

### 2025-02-25 - srcs/main.c

- **Amélioration :** Modification de la fonction `ft_process_line` pour stocker les tokens et commandes dans la structure minishell.
- **Impact :** Permet un nettoyage plus efficace des ressources mémoire.
- **Modification :** Réinitialisation des pointeurs tokens et commands à chaque nouvelle ligne traitée, et stockage des nouvelles structures créées.

- **BUGFIX** Correction de la construction de la liste des variables d'environnement
  ► Impact : Élimination des 5,730 octets de fuites mémoire signalés par Valgrind lors de l'utilisation de Ctrl+D
  ► Modification : Changement de la méthode d'ajout des nœuds dans ft_env_to_list pour construire la liste dans le bon ordre

### srcs/garbage_collector.c

- **Ajout :** Nouvelle fonction `ft_gc_remove_cmds` pour nettoyer spécifiquement les structures de commandes.
- **Impact :** Permet un nettoyage plus granulaire des ressources allouées aux commandes.
- **Modification :** Libération en profondeur des arguments, redirections et autres ressources associées aux commandes.

### includes/header.h

- **Ajout :** Déclaration de la nouvelle fonction `ft_gc_remove_cmds`.
- **Impact :** Permet l'utilisation de la fonction dans tout le programme.

### srcs/garbage_collector.c - Optimisation de la libération de mémoire
- **Optimisation :** Amélioration de la gestion de la libération mémoire pour éviter les double free.
- **Impact :** Résout le problème précédent de "double free" et améliore la stabilité.
- **Modification :** Suppression de la ligne qui ajoutait `expanded_input` au garbage collector dans `ft_tokenize`.

- **Ajout :** Nouvelle fonction `ft_gc_remove_env` pour nettoyer spécifiquement les variables d'environnement.
- **Impact :** Élimine complètement les problèmes de mémoire "still reachable" liés aux variables d'environnement.
- **Modification :** Libération explicite des variables d'environnement et des nœuds qui les contiennent.

### 2025-02-25 - srcs/builtins/builtins_utils2.c
- **Correction de fuites mémoire dans la gestion des variables d'environnement**
  - Modification de la fonction `ft_handle_unset_var` pour utiliser `ft_gc_remove` au lieu de `free` direct
  - Ajout de gestion de mémoire cohérente dans `ft_handle_export_var` avec `ft_gc_remove` et `ft_gc_add`
  - Impact: Réduction des fuites mémoire lors de la manipulation des variables d'environnement avec les commandes `unset` et `export`

### 2025-02-25 - srcs/garbage_collector.c
- [BUGFIX] Correction de la fonction ft_gc_remove_env pour libérer correctement la mémoire
  ► Impact : Élimination des 5,730 octets de fuites mémoire signalés par Valgrind lors de l'utilisation de Ctrl+D
  ► Modification : Ajout d'appels à free() après avoir retiré les pointeurs du garbage collector

### 2025-02-25 - srcs/main.c
- [BUGFIX] Correction de la construction de la liste des variables d'environnement
  ► Impact : Amélioration de la cohérence de la structure de données et préparation pour la libération de mémoire
  ► Modification : Changement de la méthode d'ajout des nœuds dans ft_env_to_list pour construire la liste dans le bon ordre

### 2025-02-25 - srcs/parser/cmd_parser.c
- [BUGFIX] Correction de fuites mémoire dans la fonction ft_init_command
  ► Impact : Réduction des fuites mémoire signalées par Valgrind
  ► Modification : Ajout de la libération de mémoire en cas d'échec d'allocation dans ft_init_command

### 2025-02-25 - srcs/parser/parser_utils.c
- [BUGFIX] Correction de fuites mémoire dans la fonction ft_create_redirection
  ► Impact : Réduction des fuites mémoire signalées par Valgrind
  ► Modification : Ajout de la libération de mémoire en cas d'échec d'allocation dans ft_create_redirection
