# Pipe Syntax Validation - Implementation Complete

**Date:** November 18, 2025
**Status:** ✅ **SUCCESSFULLY IMPLEMENTED**

## Summary

Added comprehensive pipe syntax validation to detect and properly handle invalid pipe usage, matching bash behavior.

## Invalid Pipe Syntax Detected

The following cases now properly return syntax error with exit code 2:

### 1. Pipe at Beginning
```bash
| cat
# Output: minishell: syntax error
# Exit code: 2
```

### 2. Pipe at End
```bash
echo test |
# Output: minishell: syntax error
# Exit code: 2
```

### 3. Double Pipes
```bash
echo test | | cat
# Output: minishell: syntax error
# Exit code: 2
```

### 4. Multiple Invalid Pipes
```bash
|echo|
# Output: minishell: syntax error
# Exit code: 2
```

### 5. Empty Pipes
```bash
||
# Output: minishell: syntax error
# Exit code: 2
```

## Implementation Details

### Files Modified

1. **src/parsing/command/token_to_cmd.c**
   - Added `validate_pipe_syntax()` function
   - Validates token list before command creation
   - Sets `cmdlst->syntax_error = 1` on detection

2. **main.c**
   - Fixed syntax error message printing
   - Ensures "minishell: syntax error" is displayed when token_to_cmd fails

### Validation Logic

```c
static int validate_pipe_syntax(t_token_list *toklst)
{
    // Check for pipe at beginning
    if (first_token is TK_PIPE)
        return (1);
    
    // Check for pipe at end
    if (last_token is TK_PIPE)
        return (1);
    
    // Check for consecutive pipes
    if (token is TK_PIPE && prev is TK_PIPE)
        return (1);
    
    // Check for pipe followed by pipe
    if (token is TK_PIPE && next is TK_PIPE)
        return (1);
    
    return (0);
}
```

### Integration

The validation is called in `token_to_cmd()` before any command processing:

```c
int token_to_cmd(t_token_list *toklst, t_cmd_list *cmdlst, ...)
{
    // Validate pipe syntax first
    if (validate_pipe_syntax(toklst))
    {
        cmdlst->syntax_error = 1;
        return (1);
    }
    
    // Continue with normal processing
    final_token(toklst, envlst, last_status);
    look_for_cmd(toklst->head, toklst, cmdlst);
    ...
}
```

## Bash Compliance

All test cases match bash behavior:

| Command | Bash | Minishell | Match |
|---------|------|-----------|-------|
| `\|echo\|` | syntax error | syntax error | ✅ |
| `echo \| \| cat` | syntax error | syntax error | ✅ |
| `\| cat` | syntax error | syntax error | ✅ |
| `echo test \|` | syntax error | syntax error | ✅ |
| `echo \| cat` | works | works | ✅ |
| `ls \| grep x \| wc` | works | works | ✅ |

## Test Results

### Before Implementation
```bash
$ printf '|echo|\n' | ./minishell
(no error, silent failure)
```

### After Implementation
```bash
$ printf '|echo|\n' | ./minishell
minishell: syntax error
(exit code: 2)
```

## Valid Pipe Usage (Still Works)

✅ Simple pipes: `echo hello | cat`  
✅ Pipe chains: `ls | grep x | wc -l`  
✅ Pipes with redirections: `cat < file | grep x > out`  
✅ Pipes with per-command env: `FOO=bar env | grep FOO`

## Edge Cases Handled

1. **Whitespace around pipes:** `echo test  |  cat` (valid)
2. **Multiple commands:** `echo a | cat; echo b | cat` (both valid)
3. **Empty between pipes:** Caught by validation
4. **Only pipes:** `|||` (all caught)

## Performance Impact

- **Minimal:** O(n) single pass through token list
- **Early detection:** Fails fast before command creation
- **No impact on valid commands**

## Memory Management

- ✅ No new allocations
- ✅ No memory leaks
- ✅ Proper cleanup on error

## Conclusion

Pipe syntax validation is now **complete and robust**, catching all invalid pipe patterns while preserving all valid pipe functionality. The implementation is:

- ✅ **42 Norm compliant**
- ✅ **Bash-compatible**
- ✅ **Efficient**
- ✅ **Well-tested**

This fix addresses the issue with commands like `|echo|` that were previously not detected as syntax errors.
