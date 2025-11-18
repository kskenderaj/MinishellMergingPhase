# Heredoc Implementation Changes - Complete Record

**Date**: November 18, 2025  
**Status**: REVERTED - For future reference

## Overview
Attempted to implement proper heredoc handling with:
1. Quote detection and stripping from delimiters
2. Variable expansion control based on quoted status
3. Pre-reading heredoc content during parsing (interactive mode)
4. Using stored content during execution

## Files Created

### 1. `src/parsing/command/heredoc_utils.c`
**Purpose**: Process heredoc delimiters - detect quotes and strip them

**Functions**:
- `static int has_quotes(char *str)` - Checks if string contains ' or "
- `static char *remove_quotes_heredoc(char *str)` - Removes all quotes from delimiter
- `t_heredoc_info *process_heredoc_delimiter(char *raw_delimiter)` - Main function, returns info struct

**Key Logic**:
```c
// If delimiter has quotes: strip them and set quoted=1
// If no quotes: keep as-is and set quoted=0
// This controls variable expansion later
```

### 2. `src/parsing/command/read_heredoc.c`
**Purpose**: Read heredoc content during parsing phase (interactive mode only)

**Functions**:
- `static char *append_line(char *content, char *line)` - Appends line with newline
- `static int is_delimiter(char *line, char *delimiter)` - Checks if line matches delimiter
- `char *read_heredoc_content(char *delimiter)` - Main function, reads until delimiter

**Key Logic**:
```c
// Only works in interactive mode: if (!isatty(STDIN_FILENO)) return NULL;
// Uses readline("> ") to read each line
// Handles Ctrl+C with g_sigint_status
// Returns concatenated content as single string
```

### 3. `src/parsing/command/field_split.c` (Pre-existing but kept)
**Purpose**: Split variables with spaces in command position

**Functions**:
- `static int count_words(char *str)`
- `static char *extract_word(char *str, int *pos)`
- `char **split_on_spaces(char *str)`
- `int should_split(t_segment_list *seglst)`

## Files Modified

### 1. `include/parser.h`
**Added**:
```c
// New structure for heredoc delimiter info
typedef struct s_heredoc_info
{
	char	*delimiter;  // Delimiter with quotes stripped
	int		quoted;      // 1 if original had quotes, 0 otherwise
}	t_heredoc_info;

// Added to t_file_node structure:
struct s_file_node
{
	// ... existing fields ...
	int		heredoc_quoted;    // NEW: Flag for variable expansion control
	char	*heredoc_content;  // NEW: Pre-read heredoc content (interactive)
	// ...
};

// New function prototypes:
t_heredoc_info	*process_heredoc_delimiter(char *raw_delimiter);
char			*read_heredoc_content(char *delimiter);
```

### 2. `include/executor.h`
**Modified**:
```c
// Changed signature from:
int exec_heredoc(const char *delimiter);

// To:
int exec_heredoc(const char *delimiter, int quoted);
int exec_heredoc_from_content(char *content, int quoted);  // NEW
```

### 3. `src/parsing/command/cmdlst_filelst.c`
**Modified `create_filenode()` function**:
```c
void create_filenode(char *filename, int red_type, t_file_list *filelst)
{
	t_file_node *filenode;
	t_heredoc_info *hdoc_info;  // NEW

	filenode = gc_malloc(sizeof(*filenode));
	if (!filenode)
		return;
	filenode->redir_type = red_type;
	filenode->heredoc_quoted = 0;        // NEW: Initialize
	filenode->heredoc_content = NULL;    // NEW: Initialize
	
	// NEW: Special handling for heredocs (type 6)
	if (red_type == 6)
	{
		hdoc_info = process_heredoc_delimiter(filename);
		if (hdoc_info)
		{
			filenode->filename = hdoc_info->delimiter;        // Stripped
			filenode->heredoc_quoted = hdoc_info->quoted;     // Flag
			if (isatty(STDIN_FILENO))                         // Only if interactive
				filenode->heredoc_content = read_heredoc_content(hdoc_info->delimiter);
		}
		else
			filenode->filename = filename;
	}
	else
		filenode->filename = filename;
	push_file(filelst, filenode);
	return;
}
```

### 4. `src/parsing/command/token_to_cmd.c`
**Modified `final_token()` to skip heredoc delimiter expansion**:
```c
void final_token(t_token_list *toklst, t_env_list *envlst, int last_status)
{
	t_token *token;
	t_segment_list *seglst;
	int skip_next;  // NEW

	if (!toklst)
		return ;
	token = toklst->head;
	skip_next = 0;  // NEW: Initialize flag
	while (token)
	{
		// NEW: Skip expansion for heredoc delimiters
		if (token->type == TK_HEREDOC)
		{
			skip_next = 1;
			token = token->next;
			continue;
		}
		
		if (token->type == TK_WORD)
		{
			// NEW: Skip if this is a heredoc delimiter
			if (skip_next)
			{
				skip_next = 0;
				token = token->next;
				continue;
			}
			
			// ... rest of expansion logic ...
		}
		token = token->next;
	}
}
```

### 5. `src/execution/executor_part/exec_external_handler.c`
**Added two new functions**:

```c
// Function 1: Expand variables in pre-read heredoc content
static char *expand_heredoc_content(char *content, int quoted)
{
	char *result;
	char *line;
	char *expanded;
	char *ptr;
	char *newline_pos;
	
	if (!content || quoted)
		return (content);
	
	result = gc_strdup("");
	ptr = content;
	while (*ptr)
	{
		newline_pos = ft_strchr(ptr, '\n');
		if (newline_pos)
		{
			line = gc_substr(ptr, 0, newline_pos - ptr);
			expanded = expand_heredoc_line(line, 1);
			result = gc_strjoin(result, expanded);
			result = gc_strjoin(result, "\n");
			ptr = newline_pos + 1;
		}
		else
		{
			expanded = expand_heredoc_line(ptr, 1);
			result = gc_strjoin(result, expanded);
			break;
		}
	}
	return (result);
}

// Function 2: Execute heredoc from pre-read content
int exec_heredoc_from_content(char *content, int quoted)
{
	int pipefd[2];
	char *expanded;

	if (pipe(pipefd) == -1)
		return (-1);
	expanded = expand_heredoc_content(content, quoted);
	if (expanded)
		write(pipefd[1], expanded, ft_strlen(expanded));
	close(pipefd[1]);
	return (pipefd[0]);
}
```

**Modified existing `exec_heredoc()` to handle piped input**:
```c
// Added helper for non-interactive mode
static char *read_line_from_stdin(void)
{
	char buffer[2];
	char *line;
	int bytes;

	line = gc_strdup("");
	while (1)
	{
		bytes = read(STDIN_FILENO, buffer, 1);
		if (bytes <= 0 || buffer[0] == '\n')
			return (line);
		buffer[1] = '\0';
		line = gc_strjoin(line, buffer);
		if (!line)
			return (NULL);
	}
	return (line);
}

// Modified exec_heredoc to detect tty and use appropriate read method
int exec_heredoc(const char *delimiter, int quoted)
{
	char *buffer;
	int pipefd[2];
	char *expanded;
	size_t delim_len;
	int is_tty;

	if (pipe(pipefd) == -1)
		return (-1);
	delim_len = ft_strlen(delimiter);
	is_tty = isatty(STDIN_FILENO);
	if (is_tty)
		start_heredoc_signals();
	
	while (1)
	{
		// Use readline for interactive, read() for piped
		if (is_tty)
			buffer = readline("> ");
		else
			buffer = read_line_from_stdin();
			
		if (!buffer || (is_tty && g_sigint_status == 130))
		{
			if (!buffer)
				write(STDERR_FILENO, "warning: here-document delimited by end-of-file\n", 49);
			close(pipefd[1]);
			close(pipefd[0]);
			if (is_tty && g_sigint_status == 130)
				return (-2);
			return (pipefd[0]);
		}
		
		if (ft_strncmp(buffer, delimiter, delim_len) == 0 && buffer[delim_len] == '\0')
			break;
			
		expanded = expand_heredoc_line(buffer, !quoted);
		write(pipefd[1], expanded, ft_strlen(expanded));
		write(pipefd[1], "\n", 1);
	}
	
	if (is_tty)
		start_signals();
	close(pipefd[1]);
	return (pipefd[0]);
}
```

### 6. `src/execution/executor_part/exec_redirections.c`
**Modified `setup_input_file()` to use heredoc_content**:
```c
int setup_input_file(t_commandlist *cmd)
{
	int input_count;
	t_file_node *last_input;
	int fd;

	input_count = count_input(cmd);
	if (input_count == 0)
		return (NO_REDIRECTION);
	last_input = find_last_input(cmd, input_count);
	if (last_input == NULL)
		return (NO_REDIRECTION);
	
	// NEW: Handle heredocs specially
	if (last_input->redir_type == HEREDOC)
	{
		// If content was pre-read (interactive mode), use it
		if (last_input->heredoc_content)
			return (exec_heredoc_from_content(last_input->heredoc_content,
					last_input->heredoc_quoted));
		// Otherwise fall back to reading from stdin (piped mode)
		else
			return (exec_heredoc(last_input->filename,
					last_input->heredoc_quoted));
	}
	
	// Original infile handling
	fd = gc_open(last_input->filename, O_RDONLY, 0);
	if (fd < 0)
	{
		perror(last_input->filename);
		g_shell.last_status = 1;
	}
	return (fd);
}
```

### 7. `main.c`
**Added g_shell.env initialization** (already done earlier):
```c
int main(int argc, char **argv, char **envp)
{
	t_env_list *env;
	int exit_status;

	// ... initialization ...
	env = init_environment(envp);
	if (!env)
	{
		gc_cleanup();
		return (1);
	}
	g_shell.env = env;  // CRITICAL: This line was added earlier
	exit_status = main_loop(env);
	// ... cleanup ...
	return (exit_status);
}
```

### 8. `Makefile`
**Added new source files**:
```makefile
PARSER_SRCS = \
	src/parsing/lexing/token_check.c \
	src/parsing/lexing/token_utils.c \
	src/parsing/lexing/tokenize.c \
	src/parsing/lexing/tokenize_helper.c \
	src/parsing/lexing/init.c \
	src/parsing/command/token_to_cmd.c \
	src/parsing/command/find_token.c \
	src/parsing/command/add_env.c \
	src/parsing/command/token_to_cmd_helper.c \
	src/parsing/command/expand_env.c \
	src/parsing/command/cmdlst_filelst.c \
	src/parsing/command/field_split.c \
	src/parsing/command/heredoc_utils.c \      # ADDED
	src/parsing/command/read_heredoc.c         # ADDED
```

## Architecture Design

### Flow for Interactive Mode (TTY):
1. **Parsing Phase**:
   - User enters: `cat << 'lim'`
   - Tokenizer creates TK_HEREDOC and TK_WORD('lim') tokens
   - `create_filenode()` called with type=HEREDOC, filename='lim'
   - `process_heredoc_delimiter()` detects quotes, strips them → delimiter="lim", quoted=1
   - `isatty(STDIN_FILENO)` is true, so `read_heredoc_content("lim")` is called
   - User enters heredoc lines interactively with readline("> ")
   - Content stored in `filenode->heredoc_content`
   - Command continues to execute

2. **Execution Phase**:
   - `setup_input_file()` sees HEREDOC type
   - Checks if `heredoc_content` exists (it does in interactive mode)
   - Calls `exec_heredoc_from_content(content, quoted=1)`
   - Creates pipe, writes content (no expansion since quoted=1)
   - Returns read end of pipe as input fd

### Flow for Piped Mode (Non-TTY):
1. **Parsing Phase**:
   - Input: `printf "cat << lim\ntest\nlim\n" | ./minishell`
   - First line "cat << lim" is parsed
   - `create_filenode()` called with type=HEREDOC, filename='lim'
   - `process_heredoc_delimiter()` strips quotes → delimiter="lim", quoted=0
   - `isatty(STDIN_FILENO)` is FALSE, so `read_heredoc_content()` returns NULL
   - `heredoc_content` remains NULL

2. **Execution Phase**:
   - `setup_input_file()` sees HEREDOC type
   - Checks if `heredoc_content` exists (it's NULL in piped mode)
   - Falls back to `exec_heredoc(delimiter="lim", quoted=0)`
   - Modified `exec_heredoc()` detects !isatty
   - Uses `read_line_from_stdin()` instead of `readline()`
   - Reads from stdin until delimiter found
   - Expands variables (since quoted=0)
   - Returns pipe fd

## Known Issues / Why This Was Reverted

### 1. Piped Input Architecture Problem
**Issue**: When stdin is piped, the main command loop and heredoc execution compete for the same stdin.

**Example**:
```bash
printf "cat << lim\ntest content\nlim\necho done\n" | ./minishell
```

**What happens**:
- Main loop reads: "cat << lim" (parses command)
- Main loop reads: "test content" (tries to execute as command! ❌)
- Main loop reads: "lim" (tries to execute as command! ❌)
- Main loop reads: "echo done" (finally executes correctly)

**Root Cause**: The main loop reads ONE LINE at a time, but heredocs span MULTIPLE LINES. There's no way for the parser to "pause" the main loop and tell it "the next N lines belong to this heredoc, not to you."

**Bash's Solution**: Bash doesn't read line-by-line. It reads command-by-command. When it sees `<< delimiter`, it knows to keep reading until it finds the delimiter, treating the whole thing as one command.

**Our Current Architecture**: Line-by-line reading in `main_loop()` → `read_line_noninteractive()` → `process_input_line()`

### 2. 42 Norm Compliance Challenges
Some functions approached the 25-line limit:
- `expand_heredoc_content()`: 25 lines (barely under limit)
- `exec_heredoc()`: Had to be carefully split

### 3. Test Results

**✅ Works**:
```bash
# Interactive heredoc with quotes (no expansion)
$ ./minishell
minishell$ cat << 'lim'
> $USER
> lim
$USER

# Interactive heredoc without quotes (expansion)
$ ./minishell
minishell$ cat << lim
> $USER
> lim
klejdi
```

**❌ Fails**:
```bash
# Piped heredoc
$ printf "cat << 'lim'\n\$USER\nlim\n" | ./minishell
klejdi: No such file or directory
lim: No such file or directory
```

## Proper Solution (Not Implemented)

To fully fix piped heredocs, you would need to:

1. **Refactor Input Reading**:
   - Change from line-by-line to command-by-command parsing
   - When `<<` is detected, immediately consume following lines until delimiter
   - Build complete multi-line command before processing

2. **Alternative: Buffer All Input**:
   ```c
   // In main_loop() for non-interactive mode:
   if (!isatty(STDIN_FILENO))
   {
       all_input = read_entire_stdin();
       while (parse_next_command(&all_input, &command))
       {
           process_input_line(command, env, last_status);
       }
   }
   ```

3. **Parser-Level Heredoc Awareness**:
   - Modify tokenizer to recognize heredoc spans
   - Include heredoc content as part of the token stream
   - Don't rely on stdin being available during execution

## Files to Revert

To fully revert heredoc changes:

1. **Delete created files**:
   ```bash
   rm src/parsing/command/heredoc_utils.c
   rm src/parsing/command/read_heredoc.c
   ```

2. **Revert modified files**:
   - `include/parser.h` - Remove t_heredoc_info, heredoc_quoted, heredoc_content
   - `include/executor.h` - Restore old exec_heredoc signature, remove exec_heredoc_from_content
   - `src/parsing/command/cmdlst_filelst.c` - Restore simple create_filenode
   - `src/parsing/command/token_to_cmd.c` - Remove skip_next logic
   - `src/execution/executor_part/exec_external_handler.c` - Remove new functions, restore original exec_heredoc
   - `src/execution/executor_part/exec_redirections.c` - Restore simple HEREDOC handling
   - `Makefile` - Remove heredoc_utils.c and read_heredoc.c from PARSER_SRCS

3. **Keep these changes** (they're good fixes):
   - `src/parsing/command/field_split.c` - Field splitting for command position
   - `src/parsing/command/add_env.c` - remove_from_env_list() function
   - `main.c` - g_shell.env initialization

## Lessons Learned

1. **Architecture Matters**: Trying to bolt on features without considering the fundamental input handling architecture leads to problems
2. **Interactive vs Piped**: These are fundamentally different modes that may need different code paths
3. **When to Read Content**: Content should ideally be read "as close to the source as possible" - for heredocs, that means during parsing, not execution
4. **42 Norm**: Proper modularization helps stay under line limits

## Future Reference

If you need to implement proper heredoc support later:
- Start with refactoring the input reading architecture first
- Consider how command spanning multiple lines should be handled
- Test piped input from the beginning, not as an afterthought
- Interactive mode is easier - focus on that first, then tackle piped mode

## Summary

This was a comprehensive attempt to fix heredocs properly. The solution works perfectly for **interactive mode** but has fundamental issues with **piped mode** due to architectural constraints. Reverting these changes allows you to focus on other evaluation criteria without the complexity of this partial implementation.

The code is well-documented here for future reference if you ever need to tackle this problem again with a full architectural redesign.
