# File Descriptor Restoration Bug Fix

## Problem

When executing a redirection without a command (e.g., `> outfile` or `< infile`), the shell would "get stuck" or appear to stop processing subsequent commands. In interactive mode, this made the shell seem frozen. In non-interactive mode (piped input), the shell would silently exit after processing the redirect.

### Symptoms

```bash
# This would print "1" but never print "2"
echo 1
> outfile
echo 2
```

### User Report

> "when i do '> outfile ' it gets stuck"

## Root Cause

The bug was in `main/single_cmd.c` in the `handle_single_command()` function:

```c
// OLD BUGGY CODE (lines 63-69)
if (is_empty_command(cmd))
{
    if (handle_assignment_only(cmd))
        restore_cmd_fds(&data.fds);  // ❌ Only restored conditionally!
    ft_free_array(data.envp);
    return (0);
}
```

**The Problem:**
- When a command with redirections but no actual command was processed (e.g., `> outfile`), the shell would:
  1. Save the original STDIN/STDOUT file descriptors
  2. Apply the redirections (potentially redirecting STDOUT to the file)
  3. Detect it's an empty command
  4. Call `handle_assignment_only()` which returns 0 (false) because there are no assignments
  5. **SKIP restoring the original file descriptors** because `handle_assignment_only()` returned false
  6. Return to main loop
  
- In non-interactive mode, the main loop tries to read the next line from STDIN using `read_line_noninteractive()`, but STDIN may have been redirected and never restored, causing the read to fail or behave incorrectly.

- This caused the shell to appear "stuck" or exit prematurely.

## Solution

**Fixed in:** `main/single_cmd.c` (lines 63-69)

```c
// NEW FIXED CODE
if (is_empty_command(cmd))
{
    handle_assignment_only(cmd);     // Call but ignore return value
    restore_cmd_fds(&data.fds);      // ✅ Always restore!
    ft_free_array(data.envp);
    return (0);
}
```

**Key Change:**
- **Always** restore the saved file descriptors after processing an empty command with redirections
- Don't make restoration conditional on `handle_assignment_only()` return value
- This ensures STDIN/STDOUT are always returned to their original state before continuing to the next command

## Files Modified

1. **main/input_processing.c** (lines 38-41)
   - Added syntax error message when tokenizer fails
   - This was an additional improvement discovered during investigation
   
```c
if (tokenize(&toklst, line) != 0)
{
    ft_putendl_fd("minishell: syntax error", 2);  // Added error message
    return (2);
}
```

2. **main/single_cmd.c** (lines 63-69)
   - Fixed file descriptor restoration logic (main fix)

## Testing

### Test Case 1: Redirect without command
```bash
$ echo 1
1
$ > outfile
$ echo 2
2
$ ls -l outfile
-rw-r--r-- 1 user staff 0 Nov 19 13:34 outfile
```
✅ Shell continues processing after redirect

### Test Case 2: Redirect with trailing space
```bash
$ echo START
START
$ > outfile 
$ echo REACHED
REACHED
```
✅ Trailing space doesn't cause issues

### Test Case 3: Multiple redirects
```bash
$ echo test > outfile
$ cat outfile
test
$ > empty
$ ls -l empty
-rw-r--r-- 1 user staff 0 Nov 19 13:34 empty
$ echo hello > outfile 
$ cat outfile
hello
```
✅ All redirect scenarios work correctly

### Test Case 4: Input redirect without command
```bash
$ echo "data" > infile
$ < infile
$ echo "after"
after
```
✅ Input redirects also work

## Bash Compatibility

This fix ensures our shell behaves like bash:

```bash
# In bash:
$ > somefile
$ echo $?
0
```

Bash creates the file and continues without error. Our shell now does the same.

## Technical Details

### File Descriptor Management

When redirections are set up:
1. `setup_cmd_redirections()` saves STDIN (fd 0) and STDOUT (fd 1) by calling `dup()`
2. `apply_cmd_redirections()` uses `dup2()` to redirect to the new files
3. **`restore_cmd_fds()` MUST be called** to restore original fds using `dup2()` back

If step 3 is skipped, the shell's STDIN/STDOUT remain pointing to the redirected files, breaking the main loop's ability to read subsequent input.

### Why This Caused "Getting Stuck"

In non-interactive mode:
- `read_line_noninteractive()` reads from STDIN (fd 0)
- If STDIN was redirected to a file and never restored, `read()` returns 0 (EOF)
- Shell interprets this as end of input and exits

In interactive mode:
- `readline()` also reads from STDIN
- If STDIN is redirected, readline may hang waiting for input that never comes
- User perceives shell as "stuck"

## Related Issues Fixed

During investigation, also fixed:
- Missing syntax error message when tokenizer fails
- Improved error handling in `process_input_line()`

## Date

November 19, 2025

## Impact

- ✅ Fixed shell hanging/exiting with redirect-only commands
- ✅ Proper file descriptor management for all command types
- ✅ Bash-compatible behavior for empty commands with redirections
- ✅ No regressions in existing functionality
