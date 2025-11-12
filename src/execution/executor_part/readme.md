Valid assignment (export VAR=value):

Splits at =, checks if VAR is a valid identifier, then calls setenv(VAR, value, 1) to add/update.
Mark as exported (export VAR):

Checks if VAR is valid. If getenv(VAR) exists, calls setenv(VAR, getenv(VAR), 1) (marks as exported, does not change value).
Print all (export):

If no arguments, calls print_exported_env() to print all variables in declare -x VAR="value" format.
Multiple variables (export VAR1=value1 VAR2=value2):

Loops through all arguments, applies logic for each.
Invalid identifier (export 1BAD=oops, export 1BAD):

Checks identifier validity. If invalid, prints error and skips.
Empty value (export EMPTY=):

Sets EMPTY to an empty string using setenv.
Multiple invalid/valid (export 1BAD=oops _GOOD=ok):

Prints error for 1BAD, sets _GOOD.
Underscore/digits after first char (export _UNDERSCORE=ok, export VAR1=ok):

Passes identifier check, sets variable.
Special characters (export VAR$=oops):

Fails identifier check, prints error.
Existing variable (export VAR if already set):

If VAR exists, marks as exported (does not change value).
Spaces in value (export VAR="hello world"):

Sets value to "hello world" (quotes included, no parsing).
export =oops (empty name):

Fails identifier check, prints error.
How it works:

For each argument, it checks if there’s an =. If so, splits and validates the name. If valid, sets the variable. If not, prints an error.
If no =, just validates and marks as exported if already set.
If no arguments, prints all exported variables.
THATS EXPORT EVERY DETAIL ON HOW IT WORKS I WROTE THERE

Tokenizer: Quote-aware, handles |, <, >, <<, >>, with early syntax checks.
Parser: Linked list of command nodes; redirs left in argv for executor.
Executor:
Single and pipeline execution.
Builtins run in parent with redirections; externals via fork/execve.
Unified error reporting; exit codes aligned (127 ENOENT, 126 EACCES/EISDIR, 2 syntax, 128+signal).
Redirections: Attached and separated operators; centralized syntax checks; heredoc stub.
Expansion: $VAR, $? and $<digit>→""; no expansion in single quotes; quote-aware splitting.
Environment: Linked-list env; export formatting sorted/escaped; unset removes; SHLVL increment.
Builtins: echo, cd, pwd, env (rejects args), export (sorted/escaped), unset, exit (numeric parsing, too many args, non-numeric).
Stability: Fixed pipeline segfaults; safe argv shifting to avoid OOB writes.
