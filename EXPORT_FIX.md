# Export Variable Expansion Fix

**Date:** November 18, 2025  
**Status:** ✅ **FIXED**

## Problem

`export` command was setting variables in the process environment (visible to `env`) but NOT in the shell's internal environment list (used for `$VAR` expansion).

### Symptoms:
```bash
$ export MYVAR=test123
$ env | grep MYVAR
MYVAR=test123        # ✓ Shows in env
$ echo $MYVAR
                     # ✗ Empty - variable not expanded!
```

### Root Cause:

In `exec_builtins2.c`, the `export_with_value()` function:
1. Called `setenv(name, value, 1)` - sets in process environment
2. Tracked in `g_shell.exported_vars` - for export tracking
3. **BUT:** Did NOT update `g_shell.env` - the shell's internal env list used for variable expansion

## Solution

Added `update_shell_env()` helper function that:
1. Searches `g_shell.env` for existing variable
2. Updates value if found
3. Adds new entry if not found

### Implementation:

**File:** `src/execution/executor_part/exec_builtins2.c`

```c
/* Helper to update shell's internal environment list */
static void update_shell_env(const char *name, const char *value)
{
	t_env_node *existing;
	t_env_node *new_node;

	if (!g_shell.env || !name)
		return;
	/* Check if variable already exists and update it */
	existing = g_shell.env->head;
	while (existing)
	{
		if (ft_strcmp(existing->key, name) == 0)
		{
			/* Update existing value */
			if (existing->value)
				free(existing->value);
			existing->value = ft_strdup(value);
			return;
		}
		existing = existing->next;
	}
	/* Variable doesn't exist, add new entry */
	new_node = malloc(sizeof(t_env_node));
	if (new_node)
	{
		new_node->key = ft_strdup(name);
		new_node->value = ft_strdup(value);
		push_env(g_shell.env, new_node);
	}
}
```

Modified `export_with_value()` to call `update_shell_env()`:

```c
if (stripped)
{
	setenv(name, stripped, 1);
	update_shell_env(name, stripped);  // NEW
	gc_free(stripped);
}
else
{
	setenv(name, value, 1);
	update_shell_env(name, value);     // NEW
}
```

## Test Results

### Before:
```bash
$ export PATH=1
$ echo $PATH
/usr/bin:/bin:...    # Old PATH, not updated
$ echo 1
1
```

### After:
```bash
$ export PATH=1
$ echo $PATH
1                    # ✓ Correctly shows new value
$ echo 1
1
```

### Verification:
```bash
$ export MYVAR=test123
$ echo $MYVAR
test123              # ✓ Variable expands correctly
$ env | grep MYVAR
MYVAR=test123        # ✓ Still shows in env
```

## Impact

This fix ensures that:
- ✅ Exported variables are immediately available for expansion
- ✅ `$PATH`, `$HOME`, and all other exported vars work correctly
- ✅ Both `env` command and `$VAR` expansion see the same values
- ✅ Maintains synchronization between process env and shell env

## Related Issues

### ANSI-C Quoting (`$'text'`) and Locale Translation (`$"text"`)

The following patterns are **advanced bash features** NOT required for minishell:

```bash
echo $"HOME"         # Locale-specific translation (bash outputs: HOME)
echo $'HOME'         # ANSI-C quoting (bash outputs: HOME)
echo $"VAR"$USER     # Mixed (bash: VAR + username)
```

These are NOT standard POSIX shell features and are NOT in the minishell subject requirements. Minishell only needs to support:
- ✅ `$VAR` - basic variable expansion
- ✅ `"$VAR"` - expansion inside double quotes
- ✅ `'$VAR'` - no expansion inside single quotes

The patterns like `$"..."` and `$'...'` are bash-specific extensions for:
- `$"..."` - gettext translation (for internationalization)
- `$'...'` - ANSI-C escape sequences (\\n, \\t, etc.)

**Decision:** These are out of scope for minishell and can be safely ignored.

## Files Modified

1. **src/execution/executor_part/exec_builtins2.c**
   - Added `update_shell_env()` function
   - Modified `export_with_value()` to update shell env

## Testing Commands

```bash
# Test basic export
export TEST=hello
echo $TEST           # Should output: hello

# Test PATH override
export PATH=1
echo $PATH           # Should output: 1

# Test env visibility
export MYVAR=test
env | grep MYVAR     # Should show: MYVAR=test
echo $MYVAR          # Should output: test

# Test update existing
export PATH=/usr/bin
export PATH=/bin
echo $PATH           # Should output: /bin (updated)
```

All tests pass! ✅

## Conclusion

The export command now properly updates both:
1. Process environment (via `setenv`) - for child processes
2. Shell environment (via `update_shell_env`) - for variable expansion

This ensures consistent behavior matching bash for all standard export use cases.
