# Signal Handling and Double Free Fix

**Date:** November 20, 2025  
**Issue:** Nested minishell Ctrl+C behavior and double free on exit

---

## Problems Fixed

### 1. **Nested Minishell Signal Corruption**
When running nested minishells (`./minishell` inside minishell) and pressing Ctrl+C, the prompt display was corrupted:
```
minishell$ ./minishell minishell$ ./minishell minishell$ 
```

**Root Cause:**
- Parent shell had custom signal handlers that called `rl_redisplay()`
- Child processes (nested minishells) inherited these handlers
- When Ctrl+C was pressed, ALL nested shells' handlers fired simultaneously
- This caused multiple prompts to be displayed at once

### 2. **Double Free on Exit**
When exiting from nested minishells, the program crashed with:
```
double free or corruption (fasttop)
IOT instruction (core dumped)
```

**Root Cause:**
- `env_list_to_array()` allocated memory with `gc_malloc()` (garbage collector managed)
- `cleanup_and_exit()` manually freed it with `ft_free_array()`
- Then `gc_cleanup()` tried to free the same memory again
- Result: double free corruption

---

## Changes Made

### File 1: `src/execution/executor_part/exec_external_handler.c`

**Location:** Line 19 (in `exec_external()` function)

**Change:** Added signal reset at the start of external command execution

```c
void	exec_external(char **args, char **envp)
{
	char	*exec_path;

	if (!args || !args[0])
		return ;
	reset_signals_for_child();  // ← ADDED THIS LINE
	exec_path = find_in_path(args[0]);
	// ... rest of function
}
```

**Why:** Ensures external commands (including nested minishells) start with default signal handlers, not the parent's custom readline handlers.

---

### File 2: `src/execution/executor_part/exec_pipeline_helpers.c`

**Location:** Lines 215-232 (`wait_children()` function)

**Change:** Added signal ignoring while waiting for children, and proper signal termination handling

**Before:**
```c
static int	wait_children(pid_t *pids, int ncmds)
{
	int	i;
	int	status;

	status = 0;
	i = 0;
	while (i < ncmds)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (128);
}
```

**After:**
```c
static int	wait_children(pid_t *pids, int ncmds)
{
	int	i;
	int	status;

	status = 0;
	/* Ignore signals while waiting for children - let child handle them */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	i = 0;
	while (i < ncmds)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	/* Restore interactive signal handlers */
	start_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (128);
}
```

**Why:** 
- Parent shell ignores SIGINT/SIGQUIT while waiting for foreground processes
- Only the child process handles Ctrl+C
- After children finish, parent restores interactive signal handlers
- Properly handles signal termination (Ctrl+C returns 130 = 128 + SIGINT)

---

### File 3: `src/execution/executor_part/exec_builtins3.c`

**Location:** Lines 116-124 (`cleanup_and_exit()` function)

**Change:** Removed manual freeing of `g_shell.current_envp`

**Before:**
```c
static void	cleanup_and_exit(int exit_code)
{
	if (g_shell.current_line)
		free(g_shell.current_line);
	if (g_shell.current_envp)
		ft_free_array(g_shell.current_envp);  // ← REMOVED THIS
	gc_cleanup();
	if (g_shell.env)
		free_env_list(g_shell.env);
	exit(exit_code);
}
```

**After:**
```c
static void	cleanup_and_exit(int exit_code)
{
	if (g_shell.current_line)
		free(g_shell.current_line);
	gc_cleanup();
	if (g_shell.env)
		free_env_list(g_shell.env);
	exit(exit_code);
}
```

**Why:** 
- `g_shell.current_envp` is allocated with `gc_malloc()` in `env_list_to_array()`
- The garbage collector (`gc_cleanup()`) will automatically free it
- Manual freeing with `ft_free_array()` caused double free
- Let the GC handle its own memory

---

## How It Works Now (Like Bash)

### Signal Handling Flow:

1. **Parent shell (interactive):**
   - Has custom signal handlers with `rl_redisplay()`
   - When executing a command, it IGNORES SIGINT/SIGQUIT
   - Waits for child to finish
   - After child exits, restores interactive handlers

2. **Child process:**
   - Gets DEFAULT signal handlers (SIG_DFL) via `reset_signals_for_child()`
   - Handles Ctrl+C by terminating (default behavior)
   - Parent detects signal termination and returns 130

3. **Result:**
   - Only ONE prompt after Ctrl+C
   - Clean, bash-like behavior
   - No prompt corruption in nested shells

### Memory Management:

- All `gc_malloc()` allocations are freed by `gc_cleanup()`
- No manual freeing of GC-managed memory
- Clean exit without double free errors

---

## Testing Results

✅ **Nested minishells work correctly:**
```bash
$ ./minishell
minishell$ ./minishell
minishell$ ./minishell
minishell$ ^C         # Only ONE clean prompt
minishell$ 
```

✅ **Exit works without crashes:**
```bash
minishell$ ./minishell
minishell$ exit
minishell$ exit
$                     # Clean exit, no double free
```

✅ **Valgrind shows no double free errors**

---

## Files Modified Summary

1. **`src/execution/executor_part/exec_external_handler.c`**
   - Added: `reset_signals_for_child()` call in `exec_external()`

2. **`src/execution/executor_part/exec_pipeline_helpers.c`**
   - Modified: `wait_children()` function
   - Added: Signal ignoring while waiting
   - Added: Signal restoration after waiting
   - Added: Proper WIFSIGNALED handling

3. **`src/execution/executor_part/exec_builtins3.c`**
   - Modified: `cleanup_and_exit()` function
   - Removed: Manual `ft_free_array(g_shell.current_envp)` call

---

## Important Notes for Team

⚠️ **Memory Management Rule:**
- If memory is allocated with `gc_malloc()`, let `gc_cleanup()` free it
- Don't mix GC memory with manual `free()` calls
- `g_shell.current_envp` is GC-managed, don't free it manually

⚠️ **Signal Handling Rule:**
- Parent ignores signals while waiting for children
- Children get default signal handlers
- This is how bash does it!

---

## Related Functions

These functions are already implemented in `signals.c`:
- `start_signals()` - Set up interactive signal handlers
- `reset_signals_for_child()` - Reset to default for children
- `start_heredoc_signals()` - Special handlers for heredoc

No changes needed in `signals.c` itself, just proper usage in execution flow.
