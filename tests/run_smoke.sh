#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MINISHELL="$ROOT/minishell"
TMPDIR="$ROOT/tests/tmp"
mkdir -p "$TMPDIR"

fail_count=0

run_test() {
  local name="$1"
  local input="$2"
  local expected="$3"
  local actual_file="$TMPDIR/${name}.actual"
  printf "%s" "$input" | "$MINISHELL" > "$actual_file" 2>&1 || true
  # read actual output
  actual="$(cat "$actual_file")"
  # normalize trailing newlines (strip any number of final newlines)
  expected_norm="$(printf '%s' "$expected" | sed -E ':a;N;$!ba;s/\n+$//')"
  actual_norm="$(printf '%s' "$actual" | sed -E ':a;N;$!ba;s/\n+$//')"
  if [ "$actual_norm" = "$expected_norm" ]; then
    printf "[PASS] %s\n" "$name"
  else
    printf "[FAIL] %s\n" "$name"
    echo "--- expected ---"
    printf '%s\n' "$expected_norm"
    echo "--- actual ---"
    printf '%s\n' "$actual_norm"
    echo "--- diff ---"
    diff -u <(printf '%s\n' "$expected_norm") <(printf '%s\n' "$actual_norm") || true
    fail_count=$((fail_count+1))
  fi
}

# Tests
run_test "pipeline" $'echo hello | cat\n' $'hello\n'
run_test "redir" $'echo hi > out.txt\ncat out.txt\n' $'hi\n'
run_test "heredoc" $'cat <<EOF\nline1\nline2\nEOF\n' $'line1\nline2\n'
run_test "env_assign_pipeline" $'FOO=bar /usr/bin/env | /usr/bin/grep FOO\n' $'FOO=bar\n'
run_test "builtin_redir_assign" $'FOO=x echo $FOO > out2\ncat out2\n' $'x\n'
run_test "persist_assign" $'BAR=yon\necho $BAR\n' $'yon\n'
run_test "export_with_value" $'export BAZ=qux\nenv | grep BAZ\n' $'BAZ=qux\n'

check_contains() {
  local name="$1"
  local input="$2"
  local substr="$3"
  local actual_file="$TMPDIR/${name}.actual"
  printf "%s" "$input" | "$MINISHELL" > "$actual_file" 2>&1 || true
  if grep -Fq "$substr" "$actual_file"; then
    printf "[PASS] %s\n" "$name"
  else
    printf "[FAIL] %s\n" "$name"
    printf "--- expected to contain ---\n%s\n--- actual ---\n" "$substr"
    cat "$actual_file"
    exit 2
  fi
}

check_contains "export_no_value" $'export NOP\nexport | grep NOP\n' $'declare -x NOP'

if [ "$fail_count" -ne 0 ]; then
  printf "\n%d tests failed\n" "$fail_count" >&2
  exit 2
fi

printf "All tests passed\n"
exit 0
