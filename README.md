# 🐚 Minishell

A bash-like shell implementation in C, created as part of the 42 curriculum. This project recreates a functional Unix shell with support for command execution, pipes, redirections, environment variables, and signal handling.

[![42 Project](https://img.shields.io/badge/42-Project-blue)](https://42.fr)
[![Norminette](https://img.shields.io/badge/Norminette-passing-success)](https://github.com/42School/norminette)

## 📋 Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Architecture](#architecture)
- [Built-in Commands](#built-in-commands)
- [How It Works](#how-it-works)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [Technical Details](#technical-details)
- [Contributors](#contributors)

---

## ✨ Features

### Mandatory Features
- ✅ Display a prompt and wait for user input
- ✅ Command history (using readline)
- ✅ Execute commands with absolute/relative paths or from PATH
- ✅ Handle single `'` and double `"` quotes
- ✅ Input redirection `<`
- ✅ Output redirection `>`
- ✅ Heredoc `<<`
- ✅ Append redirection `>>`
- ✅ Pipes `|` (multiple pipes supported)
- ✅ Environment variable expansion `$VAR`
- ✅ Exit status expansion `$?`
- ✅ Signal handling:
  - `Ctrl+C`: Display new prompt (doesn't quit)
  - `Ctrl+D`: Exit shell gracefully
  - `Ctrl+\`: Ignored in interactive mode
- ✅ Built-in commands:
  - `echo` with `-n` option
  - `cd` (relative or absolute path)
  - `pwd` (no options)
  - `export` (no options)
  - `unset` (no options)
  - `env` (no options)
  - `exit` (with optional exit code)

### Bonus Features
- ✅ Multiple pipes (unlimited)
- ✅ SHLVL tracking
- ✅ Heredoc with expansion control (quoted vs unquoted delimiter)
- ✅ Advanced quote parsing
- ✅ Comprehensive error handling

---

## 🚀 Installation

### Prerequisites

- GCC compiler
- GNU Readline library
- Make

### macOS Installation

```bash
# Install readline (if not already installed)
brew install readline

# Clone the repository
git clone https://github.com/kskenderaj/MinishellMergingPhase.git
cd MinishellMergingPhase

# Build the project
make

# Run minishell
./minishell
```

### Linux Installation

```bash
# Install readline development library
sudo apt-get install libreadline-dev  # Debian/Ubuntu
# or
sudo yum install readline-devel       # RedHat/CentOS

# Clone and build
git clone https://github.com/kskenderaj/MinishellMergingPhase.git
cd MinishellMergingPhase
make
./minishell
```

---

## 💻 Usage

### Basic Commands

```bash
# Start the shell
./minishell

# Run simple commands
minishell> ls -la
minishell> echo "Hello, World!"
minishell> pwd

# Navigate directories
minishell> cd /tmp
minishell> cd ..
minishell> cd ~
```

### Redirections

```bash
# Output redirection
minishell> echo "hello" > file.txt
minishell> ls -l > output.txt

# Input redirection
minishell> cat < file.txt
minishell> wc -l < file.txt

# Append redirection
minishell> echo "world" >> file.txt

# Heredoc
minishell> cat << EOF
> line 1
> line 2
> EOF
```

### Pipes

```bash
# Simple pipe
minishell> ls | grep txt

# Multiple pipes
minishell> ls -l | grep txt | wc -l
minishell> cat file.txt | grep hello | sort | uniq
```

### Environment Variables

```bash
# Display environment variables
minishell> env

# Export new variable
minishell> export MY_VAR=hello
minishell> echo $MY_VAR

# Use variables in commands
minishell> echo "User: $USER, Home: $HOME"

# Check exit status
minishell> ls
minishell> echo $?        # Should print 0
minishell> ls /nonexist
minishell> echo $?        # Should print error code
```

### Quotes

```bash
# Single quotes (literal)
minishell> echo 'Hello $USER'      # Output: Hello $USER

# Double quotes (expand variables)
minishell> echo "Hello $USER"      # Output: Hello klejdi

# Mixed quotes
minishell> echo "It's working: $PWD"
```

### Exit

```bash
# Exit with status 0
minishell> exit

# Exit with custom status
minishell> exit 42
```

---

## 🏗️ Architecture

Minishell follows a modular architecture with clear separation between parsing and execution:

```
┌─────────────────────────────────────────────────────────┐
│                     USER INPUT                          │
│                 "ls -l | grep txt"                      │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                  TOKENIZATION                           │
│  Split input into tokens: ["ls", "-l", "|", "grep"]   │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│               SYNTAX VALIDATION                         │
│  Check for syntax errors: pipes, redirections, quotes  │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│              COMMAND BUILDING                           │
│  Create command structures with args and redirections  │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│          ENVIRONMENT EXPANSION                          │
│  Replace $VAR with values, handle $? for exit status   │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                   EXECUTION                             │
│  Fork/exec for external commands, direct call built-ins│
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│               MEMORY CLEANUP                            │
│  Garbage collector frees all allocated memory          │
└─────────────────────────────────────────────────────────┘
```

---

## 🔧 Built-in Commands

### `echo [-n] [string ...]`
Prints arguments to standard output
- `-n`: Don't print trailing newline
```bash
minishell> echo Hello World
Hello World
minishell> echo -n "No newline"
No newline minishell>
```

### `cd [directory]`
Changes the current working directory
- No arguments: goes to HOME
- Updates PWD and OLDPWD environment variables
```bash
minishell> cd /tmp
minishell> cd ..
minishell> cd
```

### `pwd`
Prints the current working directory
```bash
minishell> pwd
/Users/klejdi/minishell
```

### `export [name[=value] ...]`
Sets or displays environment variables
```bash
minishell> export MY_VAR=hello
minishell> export              # Display all exported variables
```

### `unset [name ...]`
Removes environment variables
```bash
minishell> unset MY_VAR
```

### `env`
Displays all environment variables
```bash
minishell> env
USER=klejdi
HOME=/Users/klejdi
PATH=/usr/bin:/bin
...
```

### `exit [n]`
Exits the shell with optional exit code
```bash
minishell> exit
minishell> exit 42
```

---

## 🔍 How It Works

### 1. Startup Phase

When you launch minishell:

1. **Initialization**
   - Sets up global shell state (`g_shell`)
   - Detects if running in interactive mode
   - Initializes signal handlers

2. **Environment Setup**
   - Copies environment variables from parent process
   - Creates linked list of environment variables
   - Increments SHLVL (shell level)

3. **Signal Configuration**
   - `SIGINT` (Ctrl+C): Custom handler for new prompt
   - `SIGQUIT` (Ctrl+\): Ignored in interactive mode

### 2. Main Loop

The shell continuously:

```c
while (running)
{
    1. Display prompt: "minishell> "
    2. Read input with readline()
    3. Check for EOF (Ctrl+D)
    4. Add to history
    5. Parse input → tokens → commands
    6. Execute commands
    7. Clean up memory
    8. Update exit status
}
```

### 3. Parsing Pipeline

#### Step 1: Tokenization
Splits input into tokens based on:
- Whitespace (word boundaries)
- Special characters: `|`, `<`, `>`, `<<`, `>>`
- Quotes: `'` and `"`

Example:
```
Input:  echo "hello world" | grep hello > out.txt
Tokens: ["echo", "hello world", "|", "grep", "hello", ">", "out.txt"]
```

#### Step 2: Syntax Validation
Checks for errors:
- Empty pipes: `| |`
- Leading/trailing pipes: `| cmd` or `cmd |`
- Missing redirection targets: `> ` or `< `
- Unclosed quotes: `"hello`

#### Step 3: Command Building
Creates command structures:
```c
typedef struct s_command
{
    char    **args;          // Command and arguments
    t_file  *redir_list;     // Redirections
    struct  s_command *next; // Next command in pipeline
}   t_command;
```

#### Step 4: Environment Expansion
Replaces variables with values:
- `$USER` → username
- `$HOME` → home directory
- `$?` → last exit status
- `$PWD` → current directory

Rules:
- Single quotes: NO expansion (`'$USER'` stays `$USER`)
- Double quotes: YES expansion (`"$USER"` becomes `klejdi`)
- Unquoted: YES expansion (`$USER` becomes `klejdi`)

### 4. Execution Pipeline

#### Single Command Execution

```
1. Check if built-in → execute directly in parent
2. Otherwise:
   a. Fork new process
   b. Setup redirections
   c. Find executable path
   d. execve() to replace process
   e. Parent waits for child
```

#### Pipeline Execution

For `cmd1 | cmd2 | cmd3`:

```
1. Create pipes: pipe1[2], pipe2[2]
2. Fork child for cmd1:
   - stdout → pipe1[write]
   - Execute cmd1
3. Fork child for cmd2:
   - stdin  → pipe1[read]
   - stdout → pipe2[write]
   - Execute cmd2
4. Fork child for cmd3:
   - stdin  → pipe2[read]
   - Execute cmd3
5. Parent closes all pipes and waits for all children
6. Return last command's exit status
```

#### Redirection Handling

- **Input (`<`)**: `open(file, O_RDONLY)` → `dup2(fd, STDIN)`
- **Output (`>`)**: `open(file, O_WRONLY | O_CREAT | O_TRUNC)` → `dup2(fd, STDOUT)`
- **Append (`>>`)**: `open(file, O_WRONLY | O_CREAT | O_APPEND)` → `dup2(fd, STDOUT)`
- **Heredoc (`<<`)**: Create temp file, read until delimiter, `dup2(fd, STDIN)`

### 5. Memory Management

Custom **Garbage Collector** system:

```c
void *gc_malloc(size_t size)
{
    1. Allocate memory with malloc()
    2. Add pointer to global tracking list
    3. Return pointer to user
}

void gc_free_all(void)
{
    1. Iterate through tracking list
    2. Free each pointer
    3. Free list nodes
    4. Reset list for next command
}
```

Benefits:
- Centralized memory management
- Prevents memory leaks
- Single cleanup point after each command

### 6. Signal Handling

#### Ctrl+C (SIGINT)
```c
void handle_sig_int(int sig)
{
    g_sigint_status = 130;      // Set exit status
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();           // Tell readline we're on new line
    rl_replace_line("", 0);     // Clear input buffer
    rl_redisplay();             // Show fresh prompt
}
```

#### Ctrl+D (EOF)
- readline() returns NULL
- Main loop detects and prints "exit"
- Shell exits gracefully

#### Ctrl+\ (SIGQUIT)
- Ignored in interactive mode
- Handled by child processes during execution

---

## 📁 Project Structure

```
MinishellMergingPhase/
│
├── README.md                      # This file
├── Makefile                       # Build configuration
├── main.c                         # Entry point, main loop
├── globals.c                      # Global variables
│
├── include/                       # Header files
│   ├── minishell.h               # Main structures and constants
│   ├── parser.h                  # Parsing functions
│   ├── executor.h                # Execution functions
│   ├── builtins.h                # Built-in commands
│   ├── garbage_collector.h       # Memory management
│   └── libft/                    # Custom C library
│       ├── libft.h
│       ├── ft_*.c
│       └── Makefile
│
├── src/
│   ├── parsing/
│   │   ├── lexing/               # Tokenization
│   │   │   ├── tokenize.c        # Main tokenizer
│   │   │   ├── tokenize_helper.c # Token creation helpers
│   │   │   ├── token_check.c     # Syntax validation
│   │   │   ├── token_utils.c     # Token utilities
│   │   │   └── init.c            # Parser initialization
│   │   │
│   │   └── command/              # Command building
│   │       ├── token_to_cmd.c    # Convert tokens to commands
│   │       ├── token_to_cmd_helper.c
│   │       ├── find_token.c      # Token search utilities
│   │       ├── add_env.c         # Environment handling
│   │       ├── expand_env.c      # Variable expansion
│   │       ├── cmdlst_filelst.c  # Command/file list management
│   │       ├── field_split.c     # Field splitting after expansion
│   │       ├── heredoc_utils.c   # Heredoc utilities
│   │       └── read_heredoc.c    # Heredoc implementation
│   │
│   └── execution/
│       └── executor_part/
│           ├── exec_basics.c           # Main execution logic
│           ├── exec_basics1.c          # Execution helpers
│           ├── exec_builtins.c         # Built-in: cd, export, unset
│           ├── exec_builtins1.c        # Built-in: env, exit
│           ├── exec_builtins2.c        # Built-in helpers
│           ├── exec_builtins3.c        # Built-in: pwd
│           ├── exec_echo.c             # Built-in: echo
│           ├── exec_external_handler.c # External command execution
│           ├── exec_external_helpers.c # External command helpers
│           ├── exec_path.c             # PATH resolution
│           ├── exec_error.c            # Error handling
│           ├── exec_args_helpers.c     # Argument processing
│           ├── exec_pipeline_helpers.c # Pipeline execution
│           ├── exec_redir_helpers.c    # Redirection helpers
│           ├── exec_redir_attached.c   # Attached redirections (>file)
│           ├── exec_redir_separated.c  # Separated redirections (> file)
│           ├── exec_redir_heredoc.c    # Heredoc redirection
│           ├── exec_redir_infile.c     # Input redirection
│           ├── exec_redir_outfile.c    # Output redirection
│           ├── exec_redirections.c     # Main redirection handler
│           ├── exec_redirections1.c    # Redirection utilities
│           ├── exec_utility_to_run.c   # Command utility functions
│           ├── signals.c               # Signal handlers
│           └── init_shell.c            # Shell initialization
│
└── Garbage_Collector/            # Memory management system
    ├── garbage_collector.c       # Main GC functions
    ├── garbage_collector1.c      # GC helpers
    ├── garbage_collector_utils.c # GC utilities
    ├── garbage_collector_utils1.c
    └── garbage_collector_parser.c # Parser-specific GC
```

---

## 🧪 Testing

### Manual Testing

```bash
# Build and run
make && ./minishell

# Test basic commands
minishell> echo hello
minishell> ls -la
minishell> pwd

# Test pipes
minishell> ls | grep mini
minishell> cat file.txt | grep pattern | wc -l

# Test redirections
minishell> echo test > file.txt
minishell> cat < file.txt
minishell> echo append >> file.txt

# Test heredoc
minishell> cat << EOF
> line 1
> line 2
> EOF

# Test environment
minishell> export TEST=hello
minishell> echo $TEST
minishell> unset TEST

# Test quotes
minishell> echo "hello $USER"
minishell> echo 'hello $USER'

# Test signals
minishell> cat
^C                    # Press Ctrl+C (should show new prompt)
minishell> cat
^D                    # Press Ctrl+D (should exit cat)
minishell> ^D         # Press Ctrl+D (should exit shell)
```

### Memory Leak Testing

```bash
# Test with valgrind (Linux)
valgrind --leak-check=full --show-leak-kinds=all ./minishell

# Test with leaks (macOS)
leaks -atExit -- ./minishell
```

### Comparison with Bash

```bash
# Test command and compare with bash
bash -c "echo hello | cat | cat"
./minishell
minishell> echo hello | cat | cat

# Compare exit codes
bash -c "ls /nonexist; echo $?"
./minishell
minishell> ls /nonexist
minishell> echo $?
```

---

## 🔬 Technical Details

### Key Structures

```c
// Global shell state
typedef struct s_shell
{
    t_env_var   *env;           // Environment variables
    int         last_status;    // Last command exit status
    int         is_interactive; // Interactive mode flag
}   t_shell;

// Command structure
typedef struct s_command
{
    char            **args;         // Command and arguments
    t_file          *redir_list;    // List of redirections
    struct s_command *next;         // Next command in pipeline
}   t_command;

// Token structure
typedef struct s_token
{
    char            *value;         // Token value
    t_token_type    type;          // Token type
    struct s_token  *next;         // Next token
}   t_token;

// Environment variable
typedef struct s_env_var
{
    char            *key;           // Variable name
    char            *value;         // Variable value
    struct s_env_var *next;        // Next variable
}   t_env_var;
```

### Exit Status Codes

| Code | Meaning |
|------|---------|
| 0    | Success |
| 1    | General error |
| 2    | Syntax error |
| 126  | Command not executable |
| 127  | Command not found |
| 130  | Terminated by Ctrl+C (SIGINT) |
| 131  | Terminated by Ctrl+\ (SIGQUIT) |

### Compilation Flags

```makefile
CFLAGS = -Wall -Wextra -Werror
READLINE_FLAGS = -lreadline
```

### Makefile Targets

```bash
make            # Build minishell
make clean      # Remove object files
make fclean     # Remove object files and executable
make re         # Rebuild from scratch
make debug      # Build with -g flag
make sanitize   # Build with address sanitizer
```

---

## 📚 Resources

### Official Documentation
- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/)
- [POSIX Shell Specification](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- [Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)

### System Calls Used
- `fork()` - Create new process
- `execve()` - Execute program
- `wait()` / `waitpid()` - Wait for child process
- `pipe()` - Create pipe for IPC
- `dup2()` - Duplicate file descriptor
- `open()` / `close()` - File operations
- `signal()` - Set signal handler
- `getcwd()` - Get current directory
- `chdir()` - Change directory
- `getenv()` / `setenv()` - Environment variables

### Readline Functions
- `readline()` - Read line from terminal
- `add_history()` - Add line to history
- `rl_on_new_line()` - Signal readline new line
- `rl_replace_line()` - Replace current line
- `rl_redisplay()` - Redisplay prompt

---

## 🎯 Key Features Explained

### Quote Handling

```bash
# Single quotes - Literal (no expansion)
minishell> echo '$USER is $HOME'
$USER is $HOME

# Double quotes - Expansion enabled
minishell> echo "$USER is $HOME"
klejdi is /Users/klejdi

# No quotes - Expansion enabled
minishell> echo $USER is $HOME
klejdi is /Users/klejdi
```

### SHLVL Tracking

```bash
# In bash (SHLVL=1)
bash$ echo $SHLVL
1
bash$ ./minishell

# In minishell (SHLVL=2)
minishell> echo $SHLVL
2
minishell> ./minishell

# Nested minishell (SHLVL=3)
minishell> echo $SHLVL
3
```

### Heredoc Expansion

```bash
# Unquoted delimiter - Variables expanded
minishell> cat << EOF
> Hello $USER
> EOF
Hello klejdi

# Quoted delimiter - Literal text
minishell> cat << 'EOF'
> Hello $USER
> EOF
Hello $USER
```

### Pipeline Execution

```bash
# Simple pipeline
minishell> echo hello | cat
hello

# Multiple pipes
minishell> ls -l | grep txt | wc -l
3

# Complex pipeline with redirections
minishell> cat file.txt | grep pattern | sort > output.txt
```

---

## 🐛 Known Limitations

These features are NOT implemented (as per project requirements):

- `\` (backslash) for line continuation
- `;` (semicolon) for command separation
- `&&` and `||` (logical operators)
- `*` (wildcards) for pattern matching
- Command substitution `$(command)` or `` `command` ``
- Arithmetic expansion `$((expression))`
- Job control (`bg`, `fg`, `jobs`)
- Subshells `(command)`
- Brace expansion `{a,b,c}`

---

## 👥 Contributors

- **Klejdi Skenderaj** - [@kskenderaj](https://github.com/kskenderaj)

---

## 📄 License

This project is part of the 42 School curriculum and follows the school's policies on code sharing and collaboration.

---

## 🎓 Learning Outcomes

Building this minishell teaches:

✅ **Process Management**
- Creating processes with `fork()`
- Replacing processes with `execve()`
- Managing child processes with `wait()`

✅ **Inter-Process Communication**
- Pipes for data flow between processes
- File descriptor manipulation
- I/O redirection

✅ **Signal Handling**
- Catching and handling signals
- Different behavior for parent/child processes
- Integration with readline library

✅ **Memory Management**
- Dynamic memory allocation
- Preventing memory leaks
- Custom garbage collector implementation

✅ **String Parsing**
- Tokenization algorithms
- Quote handling (single, double, nested)
- State machine for parsing

✅ **Unix System Programming**
- Environment variables
- File operations
- Error handling
- Exit status codes

✅ **Software Architecture**
- Modular design
- Separation of concerns
- Clean code practices

---

## 🎉 Acknowledgments

Special thanks to:
- The 42 School community
- Bash developers for the reference implementation
- The GNU Readline library maintainers

---

## 📞 Support

For questions or issues:
- Open an issue on GitHub
- Contact via 42 intra

---

**Happy Shelling! 🐚**

---

*This project was developed as part of the 42 curriculum - November 2025*
