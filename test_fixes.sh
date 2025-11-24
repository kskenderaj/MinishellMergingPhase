#!/bin/bash

echo "=== Test 1: echo \$\"\$\" should print single \$ ==="
printf 'echo $"$"\n' | ./minishell
echo ""

echo "=== Test 2: cat <<\"\"1 should use '1' as delimiter ==="
printf 'cat <<""1\ntest line\n1\n' | ./minishell
echo ""

echo "=== Test 3: Mixed quotes in heredoc delimiter ==="
printf 'cat <<a"bc"d\ntest\nabcd\n' | ./minishell
echo ""

echo "=== Manual test needed: Ctrl-C in heredoc ==="
echo "Run ./minishell and type:"
echo "  cat <<EOF"
echo "  test"
echo "  ^C  (press Ctrl-C)"
echo "Expected: should return to prompt immediately without running cat"
echo ""
