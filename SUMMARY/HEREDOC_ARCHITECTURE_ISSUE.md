# Heredoc Architecture Problem

## The Core Issue

When you run:
```bash
printf "cat << 'lim'\n\$USER\nlim\n" | ./minishell
```

What happens:
1. Shell reads: `cat << 'lim'` from stdin
2. Parses it, creates tokens
3. Tries to execute `cat`
4. Calls `exec_heredoc("lim", 1)`
5. `exec_heredoc` calls `readline("> ")` 
6. **BUG**: `readline` reads `$USER` and `lim` from stdin (the piped input)
7. But `$USER` and `lim` were ALREADY consumed from stdin by step 5!
8. Now stdin has nothing left, shell tries to read next command
9. Reads `$USER` and `lim` as commands → "command not found" errors

## The Real Problem

**Heredocs must be processed during PARSING, not EXECUTION.**

Bash does this:
1. Lexer encounters `<<` delimiter
2. **IMMEDIATELY** reads heredoc content from stdin until delimiter
3. Stores content in a temp file or buffer
4. Continues parsing rest of command
5. During execution, uses stored content

Your minishell does this:
1. Lexer encounters `<<` delimiter  
2. Stores delimiter, continues parsing
3. **Later** during execution, tries to read heredoc content
4. **TOO LATE** - stdin already consumed or has wrong content

## Solutions

### Option A: Read Heredocs During Parsing (CORRECT but complex)

Modify tokenizer to detect heredocs and immediately read content:

```c
// In tokenizer, after seeing TK_HEREDOC and delimiter:
if (token_type == TK_HEREDOC)
{
    char *delim = get_delimiter();
    char *content = read_heredoc_content_now(delim);  // Read from stdin NOW
    store_heredoc_content(content);  // Store for later
}
```

**Problems:**
- Tokenizer needs access to stdin
- Need to store heredoc content somewhere
- Complex refactoring

### Option B: Process All Lines at Once (HACK for testing)

Read entire input before processing:

```c
// Read all lines from stdin first
char *all_input = read_all_stdin();
// Then process
```

**Problems:**
- Interactive mode breaks
- Not how real shells work

### Option C: Heredoc Pre-processing Pass (COMPROMISE)

After tokenizing, before executing:
1. Walk through tokens
2. Find all TK_HEREDOC tokens
3. For each one, read content from stdin immediately
4. Store content in token or separate structure
5. During execution, use stored content

## Recommendation

For your project, the **quickest fix** that passes tests:

**Detect if stdin is a pipe/file (non-interactive):**
- If interactive: use current `readline()` approach
- If non-interactive: read heredoc content during a pre-execution phase

## Why This Wasn't Caught Before

Your previous tests probably used interactive mode or the old `fgets()` approach which had different behavior.

