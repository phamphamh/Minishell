#!/bin/bash

# Couleurs pour les logs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Début des tests de heredoc ===${NC}"

# Test 1: Cas simple de heredoc avec pipe vers ls
echo -e "\n${YELLOW}Test 1: heredoc basique avec pipe vers ls${NC}"
echo -e "Commande: << eof cat | ls\n"

echo "Quelques lignes de texte
pour tester le heredoc
avec cat
" > input_test1.txt

echo -e "${GREEN}Résultat attendu: le heredoc devrait capturer l'entrée jusqu'à 'eof' puis l'envoyer à cat, et ls devrait afficher les fichiers du répertoire${NC}"
echo -e "${YELLOW}Exécution...${NC}"
./minishell << EOF
<< eof cat | ls
Quelques lignes de texte
pour tester le heredoc
avec cat
eof
exit
EOF

# Test 2: Cas complexe avec double pipe
echo -e "\n${YELLOW}Test 2: heredoc avec double pipe${NC}"
echo -e "Commande: << eof cat | tr ' ' '_' | cat\n"

echo "Voici un texte avec des espaces
qui devraient être remplacés par
des underscores
" > input_test2.txt

echo -e "${GREEN}Résultat attendu: le heredoc devrait capturer l'entrée, cat l'envoie à tr qui remplace les espaces par des underscores, puis cat affiche le résultat${NC}"
echo -e "${YELLOW}Exécution...${NC}"
./minishell << EOF
<< eof cat | tr ' ' '_' | cat
Voici un texte avec des espaces
qui devraient être remplacés par
des underscores
eof
exit
EOF

# Test 3: Test avec plusieurs heredocs consécutifs
echo -e "\n${YELLOW}Test 3: Plusieurs heredocs consécutifs${NC}"
echo -e "Commande: << end1 cat | << end2 cat\n"

echo -e "${GREEN}Résultat attendu: Le shell devrait gérer correctement les heredocs multiples${NC}"
echo -e "${YELLOW}Exécution...${NC}"
./minishell << EOF
<< end1 cat | << end2 cat
Contenu du premier heredoc
end1
Contenu du deuxième heredoc
end2
exit
EOF

# Test 4: Heredoc avec expansions de variables
echo -e "\n${YELLOW}Test 4: Heredoc avec expansions de variables${NC}"
echo -e "Commande: << eof cat\n"

echo -e "${GREEN}Résultat attendu: Les variables d'environnement devraient être expandues correctement${NC}"
echo -e "${YELLOW}Exécution...${NC}"
./minishell << EOF
export TEST_VAR="Ceci est une variable"
<< eof cat
Texte normal
\$TEST_VAR (avec échappement, ne devrait pas être expansé)
$TEST_VAR (sans échappement, devrait être expansé)
eof
exit
EOF

echo -e "\n${YELLOW}=== Fin des tests de heredoc ===${NC}"

# Nettoyage
rm -f input_test1.txt input_test2.txt
