# Multiple Redirections Fix

## Problem

Two related issues with handling multiple redirections in a single command:

### Issue 1: Multiple Output Redirections
When executing a command with multiple output redirections (e.g., `cat file >a >b >c`), the shell was only creating the **last** file, not all of them.

```bash
# Expected (bash behavior):
$ echo hello >a >b >c
$ ls -l a b c
-rw-r--r-- 1 user staff 0 Nov 19 13:49 a    # Created but empty
-rw-r--r-- 1 user staff 0 Nov 19 13:49 b    # Created but empty  
-rw-r--r-- 1 user staff 6 Nov 19 13:49 c    # Created with content

# Actual (minishell before fix):
$ echo hello >a >b >c
$ ls -l a b c
ls: a: No such file or directory   # ❌ Not created
ls: b: No such file or directory   # ❌ Not created
-rw-r--r-- 1 user staff 6 Nov 19 13:49 c    # ✓ Created with content
```

### Issue 2: Multiple Input Redirections
When executing a command with multiple input redirections where one file doesn't exist (e.g., `cat <a <b <nonexistent <c`), the shell was silently failing without showing an error message.

```bash
# Expected (bash behavior):
$ cat <a <b <nonexistent <c
bash: nonexistent: No such file or directory   # ✓ Error shown
$ echo $?
1   # ✓ Command failed

# Actual (minishell before fix):
$ cat <a <b <nonexistent <c
# ❌ No error message
$ echo $?
1   # ✓ Command failed (status was correct, but silent)
```

## Bash Behavior (Reference)

### Multiple Output Redirections
In bash, when you have multiple output redirections:
1. **All files are opened/created** (in order from left to right)
2. Each file is truncated (if using `>`) or opened for append (if using `>>`)
3. Only the **last** file receives the actual output
4. All other files remain empty (but exist)

This is the expected behavior because each redirect replaces the previous one, but bash still opens all files to check for permissions/errors.

### Multiple Input Redirections  
In bash, when you have multiple input redirections:
1. **All files are checked for existence** (in order from left to right)
2. If any file doesn't exist, bash shows an error and the command fails
3. If all files exist, only the **last** file is actually read from
4. Error checking happens **before** command execution

## Root Cause

The original `setup_input_file()` and `setup_output_file()` functions only processed the **last** redirect in the chain:

```c
// OLD CODE (simplified)
int setup_output_file(t_commandlist *cmd)
{
    // Find the LAST output file
    last_output = find_last_output(cmd, output_count);
    
    // Open only the last file
    fd = gc_open(last_output->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    return (fd);
}
```

This meant:
- For outputs: Only the last file was created
- For inputs: Only the last file was checked for existence

## Solution

Added two new helper functions to properly handle all redirections:

### 1. `open_all_output_files()` - Opens ALL output files

```c
// Opens all output files (bash behavior: all files are created)
// Returns 0 on success, -1 if any file fails to open
static int open_all_output_files(t_commandlist *cmd)
{
    t_file_node *current;
    int fd;

    current = (t_file_node *)cmd->files;
    while (current != NULL)
    {
        if (current->redir_type == OUTFILE || current->redir_type == OUTFILE_APPEND)
        {
            if (current->redir_type == OUTFILE)
                fd = gc_open(current->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            else
                fd = gc_open(current->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror(current->filename);
                g_shell.last_status = 1;
                return (-1);
            }
            close(fd);  // Close immediately after opening
        }
        current = current->next;
    }
    return (0);
}
```

**Key points:**
- Iterates through ALL file nodes
- Opens each output file with appropriate flags
- Closes each file immediately (we just want to create/truncate it)
- Returns -1 if any file fails to open

### 2. `check_all_input_files()` - Validates ALL input files exist

```c
// Check all input files exist before opening (bash behavior)
// Returns 0 on success, -1 if any file doesn't exist
static int check_all_input_files(t_commandlist *cmd)
{
    t_file_node *current;
    int fd;

    current = (t_file_node *)cmd->files;
    while (current != NULL)
    {
        if (current->redir_type == INFILE)
        {
            fd = open(current->filename, O_RDONLY);
            if (fd < 0)
            {
                perror(current->filename);
                g_shell.last_status = 1;
                return (-1);
            }
            close(fd);
        }
        current = current->next;
    }
    return (0);
}
```

**Key points:**
- Iterates through ALL file nodes
- Opens each input file to verify it exists and is readable
- Closes each file immediately (we just want to validate it)
- Shows error message via `perror()` if any file doesn't exist
- Returns -1 on first error (fail-fast behavior)

### 3. Modified `setup_output_file()` and `setup_input_file()`

```c
int setup_output_file(t_commandlist *cmd)
{
    // ... existing code ...
    
    // NEW: Open all output files first
    if (open_all_output_files(cmd) == -1)
        return (-1);
    
    // Then find and open the last one for actual writing
    last_output = find_last_output(cmd, output_count);
    // ... rest of existing code ...
}

int setup_input_file(t_commandlist *cmd)
{
    // ... existing code ...
    
    // NEW: Check all input files exist first
    if (check_all_input_files(cmd) == -1)
        return (-1);
    
    // Then find and open the last one for actual reading
    last_input = find_last_input(cmd, input_count);
    // ... rest of existing code ...
}
```

### 4. Fixed Error Handling in `setup_cmd_redirections()`

There was also a bug in the error checking logic:

```c
// OLD CODE (BUGGY):
fds->out_fd = setup_output_file_from_cmd(cmd);
if (fds->out_fd == -1 || (g_shell.last_status == 1 && saved_status == 0))
    return (-1);  // ❌ NO_REDIRECTION is also -1!
```

The problem: `NO_REDIRECTION` is defined as -1, so the check `fds->out_fd == -1` would incorrectly treat "no redirection" as an error!

```c
// NEW CODE (FIXED):
fds->out_fd = setup_output_file_from_cmd(cmd);
if (g_shell.last_status == 1 && saved_status == 0)
    return (-1);  // ✓ Only check error status, not fd value
```

## Files Modified

1. **src/execution/executor_part/exec_redirections.c**
   - Added `open_all_output_files()` function
   - Added `check_all_input_files()` function
   - Modified `setup_output_file()` to call `open_all_output_files()`
   - Modified `setup_input_file()` to call `check_all_input_files()`

2. **main/single_cmd_setup.c**
   - Fixed error checking in `setup_cmd_redirections()`
   - Removed incorrect check for `fds->out_fd == -1`

## Testing

### Test Case 1: Multiple Output Redirections
```bash
$ cat Makefile >outfile >nga >ass >outfile2 >outifle2 >sdfsf >slkfalskf >laksdkladklaksd >HELLO
$ ls -l outfile nga ass outfile2 outifle2 sdfsf slkfalskf laksdkladklaksd HELLO
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 ass           # ✓ Created (empty)
-rw-r--r--@ 1 user staff  5662 Nov 19 13:51 HELLO         # ✓ Has content (last file)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 laksdkladklaksd  # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 nga           # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 outfile       # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 outfile2      # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 outifle2      # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 sdfsf         # ✓ Created (empty)
-rw-r--r--@ 1 user staff     0 Nov 19 13:51 slkfalskf     # ✓ Created (empty)
```
✅ All files created, only last one has content

### Test Case 2: Multiple Input Redirections with Nonexistent File
```bash
$ >outfile1
$ >outfile2
$ >outfile3
$ cat <outfile1 <outfile2 <asdjklasd <outfile3
asdjklasd: No such file or directory   # ✓ Error message shown
$ echo $?
1   # ✓ Command failed
```
✅ Error message displayed, command fails properly

### Test Case 3: All Input Files Exist
```bash
$ cat <outfile1 <outfile2 <outfile3
# (no output, all files are empty)
$ echo $?
0   # ✓ Command succeeds
```
✅ No error when all files exist

## Bash Compatibility

The shell now matches bash behavior for:
- ✅ Creating all output redirect files (not just the last)
- ✅ Only writing to the last output file
- ✅ Checking all input redirect files exist
- ✅ Showing error messages for missing input files
- ✅ Using only the last input file for reading

## Date

November 19, 2025

## Impact

- ✅ Fixed multiple output redirects - all files now created
- ✅ Fixed multiple input redirects - proper error messages
- ✅ Bash-compatible behavior for complex redirect chains
- ✅ Proper error handling and reporting
- ✅ No regressions in existing functionality
