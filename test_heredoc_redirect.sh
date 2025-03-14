#!/bin/bash

# Couleurs pour les logs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Test de heredoc avec redirection ===${NC}"

# Nettoyage des fichiers de test précédents
rm -f test_out.txt test_append.txt

# Test 1: Here-document avec redirection de sortie (>)
echo -e "\n${YELLOW}Test 1: << eof cat > test_out.txt${NC}"
./minishell << EOF
<< eof cat > test_out.txt
Ceci est un test de heredoc
avec redirection de sortie (>)
eof
exit
EOF

if [ -f "test_out.txt" ]; then
    echo -e "${GREEN}✅ Fichier test_out.txt créé${NC}"
    echo -e "${YELLOW}Contenu du fichier:${NC}"
    cat test_out.txt
else
    echo -e "${RED}❌ Le fichier test_out.txt n'a pas été créé${NC}"
fi

# Test 2: Here-document avec redirection de sortie en mode append (>>)
echo -e "\n${YELLOW}Test 2: << eof cat >> test_append.txt${NC}"
./minishell << EOF
<< eof cat >> test_append.txt
Première partie du heredoc
avec redirection append
eof
exit
EOF

./minishell << EOF
<< eof cat >> test_append.txt
Deuxième partie du heredoc
avec redirection append (>>)
eof
exit
EOF

if [ -f "test_append.txt" ]; then
    echo -e "${GREEN}✅ Fichier test_append.txt créé${NC}"
    echo -e "${YELLOW}Contenu du fichier:${NC}"
    cat test_append.txt
else
    echo -e "${RED}❌ Le fichier test_append.txt n'a pas été créé${NC}"
fi

echo -e "\n${YELLOW}=== Fin des tests ===${NC}"
