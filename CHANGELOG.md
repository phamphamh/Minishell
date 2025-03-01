# Historique des modifications - Projet 42

### 2025-02-28 - srcs/signal_handler.c
- [BUGFIX] Amélioration de la gestion des signaux pour corriger les problèmes avec Ctrl+C et Ctrl+D
  ► Impact : Comportement plus stable et conforme aux attentes avec les combinaisons de touches
  ► Modifications :
    - Ajout d'une variable globale g_signal_received pour suivre l'état des signaux
    - Réinitialisation correcte des gestionnaires de signaux après chaque commande
    - Ajout du flag SA_RESTART pour éviter les interruptions inattendues

### 2025-02-28 - srcs/main.c
- [BUGFIX] Correction de la boucle principale pour une meilleure gestion des signaux
  ► Impact : Évite les boucles infinies avec Ctrl+C et prévient la sortie inattendue après multiple Ctrl+C
  ► Modifications :
    - Réinitialisation de l'état des signaux à des moments stratégiques
    - Ajout d'une vérification explicite après readline pour les signaux reçus
    - Meilleure gestion des ressources en cas d'interruption par signal

### 2025-02-27 - srcs/parser/check_syntax.c
- [BUGFIX] Amélioration de la vérification syntaxique pour les opérateurs spéciaux
  ► Impact : Gestion correcte des erreurs syntaxiques
  ► Modification : Ajout de vérifications pour "&&", ";", et "()" qui renvoient maintenant "syntax error" au lieu de "command not found"

### 2025-02-27 - srcs/exec/exec.c
- [BUGFIX] Correction du traitement des commandes de type répertoire
  ► Impact : Message d'erreur approprié "is a directory" avec code 126
  ► Modification : Ajout de vérification stat() pour détecter les répertoires

- [BUGFIX] Amélioration de la gestion des guillemets vides comme commandes
  ► Impact : Message d'erreur plus cohérent "command not found" avec code 127
  ► Modification : Détection spécifique de "" et '' comme commandes

- [BUGFIX] Meilleure gestion des erreurs d'exécution
  ► Impact : Messages d'erreur plus précis
  ► Modification : Remplacement de l'erreur générique par utilisation de perror()

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

### 2025-03-01 - srcs/signal_handler.c
- [BUGFIX] Correction de la gestion des signaux pour Ctrl+C
  ► Impact : Comportement plus stable du shell avec les signaux
  ► Modification : Suppression de la référence à rl_replace_line qui n'est pas disponible

### 2025-03-01 - srcs/exec/redirection.c
- [BUGFIX] Ajout de la gestion des here-documents (<<)
  ► Impact : Les here-documents fonctionnent maintenant correctement
  ► Modification : Implémentation complète de la lecture des here-documents avec gestion des signaux

### 2025-03-01 - srcs/parser/cmd_parser.c
- [BUGFIX] Ajout de la prise en compte des here-documents dans le parser
  ► Impact : Les here-documents sont correctement reconnus et traités
  ► Modification : Ajout de TOKEN_HEREDOC dans les conditions de traitement des redirections

### 2025-03-01 - srcs/parser/parser_utils.c
- [BUGFIX] Correction de la fonction ft_process_redirection pour gérer toutes les redirections
  ► Impact : Toutes les redirections sont maintenant correctement traitées
  ► Modification : Ajout d'une gestion en liste chaînée pour conserver toutes les redirections

- [BUGFIX] Correction de la fonction ft_count_args pour prendre en compte les here-documents
  ► Impact : Le comptage des arguments est maintenant correct avec les here-documents
  ► Modification : Ajout de TOKEN_HEREDOC dans les conditions de comptage

### 2025-03-01 - srcs/parser/tokenizer_utils.c
- [BUGFIX] Correction de la tokenisation pour les opérateurs à deux caractères (<< et >>)
  ► Impact : Les heredocs (<<) et les redirections d'ajout (>>) fonctionnent maintenant correctement
  ► Modification : Refactorisation des fonctions ft_expand_operators et ft_fill_expanded pour traiter les opérateurs à deux caractères comme des unités
