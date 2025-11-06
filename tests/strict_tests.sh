#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MINISHELL="$ROOT/minishell"
TMP="$ROOT/tests/strict_tmp"
mkdir -p "$TMP"

fail=0

run_case() {
  name="$1"
  input_file="$TMP/${name}.in"
  actual_file="$TMP/${name}.out"
  expected_file="$TMP/${name}.exp"
  shift
  # create input: interpret backslash escapes (\n) into real newlines
  printf "%b" "$*" > "$input_file"
  # run minishell with the input
  printf "%s" "$(cat "$input_file")" | "$MINISHELL" > "$actual_file" 2>&1 || true
  if cmp -s "$actual_file" "$expected_file"; then
    printf "[PASS] %s\n" "$name"
  else
    printf "[FAIL] %s\n" "$name"
    echo "--- expected ---"
    cat "$expected_file"
    echo "--- actual ---"
    cat "$actual_file"
    fail=$((fail+1))
  fi
}

# helper to write expected file
expect() {
  name="$1"
  shift
  printf "%b" "$*" > "$TMP/${name}.exp"
}

# Define tests
# pipeline_basic
expect pipeline_basic "hello\n"
run_case pipeline_basic "echo hello | cat\n"

# pipeline_chain
expect pipeline_chain "two\n"
run_case pipeline_chain "echo one two | awk '{print \$2}'\n"

# redir_overwrite
expect redir_overwrite "hi\n"
run_case redir_overwrite "echo hi > $TMP/out1\nexit\n" && true
if [ -f "$TMP/out1" ]; then
  if ! cmp -s "$TMP/out1" "$TMP/redir_overwrite.exp"; then
    printf "[FAIL] redir_overwrite\n"
    echo "--- expected file ---"
    cat "$TMP/redir_overwrite.exp"
    echo "--- actual file ---"
    cat "$TMP/out1"
    fail=$((fail+1))
  else
    printf "[PASS] redir_overwrite\n"
  fi
else
  printf "[FAIL] redir_overwrite (no file)\n"
  fail=$((fail+1))
fi

# redir_append
expect redir_append "a\nb\n"
run_case redir_append "echo a > $TMP/out2\necho b >> $TMP/out2\nexit\n"
if [ -f "$TMP/out2" ]; then
  if ! cmp -s "$TMP/out2" "$TMP/redir_append.exp"; then
    printf "[FAIL] redir_append\n"
    echo "--- expected file ---"
    cat "$TMP/redir_append.exp"
    echo "--- actual file ---"
    cat "$TMP/out2"
    fail=$((fail+1))
  else
    printf "[PASS] redir_append\n"
  fi
else
  printf "[FAIL] redir_append (no file)\n"
  fail=$((fail+1))
fi

# heredoc_basic
expect heredoc_basic "L1\nL2\n"
run_case heredoc_basic "cat <<EOF\nL1\nL2\nEOF\n"

# per_command_env
expect per_command_env "FOO=bar\n"
run_case per_command_env "FOO=bar env | grep FOO\n"

# per_command_not_persist
expect per_command_not_persist "temp\n"
run_case per_command_not_persist "FOO=temp\necho \$FOO\n"

# export_sets_env
expect export_sets_env "XO=YY\n"
run_case export_sets_env "export XO=YY\nenv | grep XO\n"

# export_no_value
expect export_no_value "declare -x NEWVAR\n"
run_case export_no_value "export NEWVAR\nexport | grep NEWVAR\n"

# unset_removes
expect unset_removes "\n"
run_case unset_removes "export TST=1\nunset TST\nenv | grep TST\n"

# quoting
expect quoted_echo "a b\n"
run_case quoted_echo "echo \"a b\"\n"

expect single_quote_literal "a\$B\n"
run_case single_quote_literal "echo '\''a\$B'\''\n"

expect cmd_not_found "command not found\n"
run_case cmd_not_found "nosuchcommand\n"
if grep -Fq "command not found" "$TMP/cmd_not_found.out" 2>/dev/null || true; then
  printf "[PASS] cmd_not_found\n"
else
  printf "[FAIL] cmd_not_found\n"
  fail=$((fail+1))
fi
  printf "\n%d strict tests failed\n" "$fail" >&2
  exit 2
fi
printf "All strict tests passed\n"
exit 0
