#!/bin/bash

# Couleurs pour l'affichage
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}===== Test des heredocs dans Minishell =====${NC}"

# Cas 1: Heredoc simple avec pipe vers ls
echo -e "\n${GREEN}Test case 1: << eof cat | ls${NC}"
./minishell << EOF
<< eof cat | ls
ceci est un test de heredoc
multiple lignes
eof
exit
EOF

# Cas 2: Heredoc avec double pipe
echo -e "\n${GREEN}Test case 2: << eof cat | tr ' ' '_' | cat${NC}"
./minishell << EOF
<< eof cat | tr ' ' '_' | cat
Ce texte devrait avoir ses espaces
remplacés par des underscores
eof
exit
EOF

echo -e "\n${GREEN}===== Tests terminés =====${NC}"
