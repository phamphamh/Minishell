#!/bin/bash

MINISHELL_EXEC="./minishell"
BASH_EXEC="/bin/bash --posix"
TEST_FILE="tests/tests.txt"
LOG_FILE="tests/test_results.log"

echo "=== Running Minishell Tests ===" > $LOG_FILE
echo "" > $LOG_FILE

total_tests=0
passed_tests=0

while IFS= read -r test_case || [ -n "$test_case" ]; do
    total_tests=$((total_tests + 1))

    echo "--------------------------------------" | tee -a $LOG_FILE
    echo "Test: $test_case" | tee -a $LOG_FILE
    echo "--------------------------------------" | tee -a $LOG_FILE

    # Exécuter Minishell, supprimer la première ligne (la commande entrée) et enlever le prompt
    minishell_output=$( (echo "$test_case" | $MINISHELL_EXEC 2>&1) | sed -E '1d' | sed 's/^minishell\$ //g')

    # Exécuter Bash POSIX
    bash_output=$(echo "$test_case" | $BASH_EXEC 2>&1)

    # Comparer les sorties
    if [ "$minishell_output" == "$bash_output" ]; then
        echo "✅ Passed" | tee -a $LOG_FILE
        passed_tests=$((passed_tests + 1))
    else
        echo "❌ Failed" | tee -a $LOG_FILE
        echo "Minishell Output:" | tee -a $LOG_FILE
        echo "$minishell_output" | tee -a $LOG_FILE
        echo "Bash Output:" | tee -a $LOG_FILE
        echo "$bash_output" | tee -a $LOG_FILE
    fi

    echo "" | tee -a $LOG_FILE

done < "$TEST_FILE"

echo "=== Summary ===" | tee -a $LOG_FILE
echo "Total tests: $total_tests" | tee -a $LOG_FILE
echo "Passed: $passed_tests" | tee -a $LOG_FILE
echo "Failed: $((total_tests - passed_tests))" | tee -a $LOG_FILE
