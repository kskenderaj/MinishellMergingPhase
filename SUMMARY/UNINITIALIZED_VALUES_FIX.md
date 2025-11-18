# Uninitialized Values Fix

## Issue Description
Valgrind detected "Conditional jump or move depends on uninitialised value(s)" warnings in the shell, specifically in functions related to file redirections:
- `count_output` 
- `setup_output_file`
- `count_input`
- `setup_input_file`

## Root Cause
In `src/execution/executor_part/exec_redirections.c`, two wrapper functions were creating stack-allocated `t_commandlist` structures without initializing them:

```c
int setup_input_file_from_cmd(t_cmd_node *cmd)
{
    t_commandlist tmp;  // ⚠️ Uninitialized structure on stack
    
    if (!cmd || !cmd->files || !cmd->files->head)
        return (NO_REDIRECTION);
    tmp.files = (t_filelist *)cmd->files->head;  // Only one field set
    return (setup_input_file(&tmp));  // ⚠️ Other fields contain garbage
}
```

The problem: While `tmp.files` was explicitly set, all other fields in the `t_commandlist` structure contained garbage values from the stack. When `setup_input_file()` or `setup_output_file()` accessed these structures, they potentially read uninitialized memory.

## Solution
Added `ft_memset()` to zero-initialize the entire structure before use:

```c
int setup_input_file_from_cmd(t_cmd_node *cmd)
{
    t_commandlist tmp;
    
    if (!cmd || !cmd->files || !cmd->files->head)
        return (NO_REDIRECTION);
    ft_memset(&tmp, 0, sizeof(t_commandlist));  // ✅ Initialize all fields to 0
    tmp.files = (t_filelist *)cmd->files->head;
    return (setup_input_file(&tmp));
}

int setup_output_file_from_cmd(t_cmd_node *cmd)
{
    t_commandlist tmp;
    
    if (!cmd || !cmd->files || !cmd->files->head)
        return (NO_REDIRECTION);
    ft_memset(&tmp, 0, sizeof(t_commandlist));  // ✅ Initialize all fields to 0
    tmp.files = (t_filelist *)cmd->files->head;
    return (setup_output_file(&tmp));
}
```

## Files Modified
- `src/execution/executor_part/exec_redirections.c` (2 functions)

## Testing
✅ Redirections work correctly:
- Input redirections (`<`)
- Output redirections (`>`)
- Append redirections (`>>`)
- Heredocs (`<<`)

✅ No functional changes - only initialization fix
✅ Build successful with no warnings

## Prevention
**Best Practice:** Always initialize stack-allocated structures, especially when:
1. Only setting some fields explicitly
2. Passing the structure to functions that might read any field
3. Using structures with multiple fields or nested data

**Methods:**
```c
// Method 1: Zero-initialize with memset
t_commandlist tmp;
ft_memset(&tmp, 0, sizeof(t_commandlist));

// Method 2: Initialize at declaration (C99)
t_commandlist tmp = {0};

// Method 3: Initialize individual fields
t_commandlist tmp;
tmp.field1 = NULL;
tmp.field2 = 0;
// ... initialize all fields
```

## Date Fixed
November 18, 2025
