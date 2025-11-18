# 🐚 Minishell Complete Flow Guide

## 📋 Table of Contents
1. [Project Overview](#project-overview)
2. [Startup Phase](#startup-phase)
3. [Main Loop Architecture](#main-loop-architecture)
4. [Parsing Pipeline](#parsing-pipeline)
5. [Execution Pipeline](#execution-pipeline)
6. [Signal Handling](#signal-handling)
7. [Memory Management](#memory-management)
8. [Key Features](#key-features)

---

## 🎯 Project Overview

**Minishell** is a simplified bash-like shell that implements:
- Command parsing with quotes and redirections
- Pipeline execution (commands connected with `|`)
- Heredocs (`<<`)
- Environment variable expansion (`$VAR`, `$?`)
- Built-in commands (cd, echo, pwd, export, unset, env, exit)
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+\\)
- Shell level tracking (SHLVL)

---

## 🚀 Startup Phase

### 1. **main.c - Entry Point**

```c
int main(int ac, char **av, char **envp)
```

**Steps:**
1. **Initialize Global Shell State** (`init_shell()`)
   - Sets up `g_shell` structure
   - Checks if running in interactive mode (`isatty(STDIN_FILENO)`)
   - Initializes last_status to 0

2. **Setup Signal Handlers** (`setup_signal_handlers()`)
   - SIGINT (Ctrl+C): Custom handler
   - SIGQUIT (Ctrl+\\): Ignore in interactive mode

3. **Initialize Environment** (`init_environment(envp)`)
   - Copies environment variables into linked list
   - Each node: `t_env_var` with `key=value`

4. **Increment SHLVL** (`increment_shlvl()`)
   - Gets current SHLVL from environment
   - Increments by 1
   - Updates environment with new value
   - Example: bash SHLVL=2 → minishell SHLVL=3

5. **Enter Main Loop**

---

## 🔄 Main Loop Architecture

### Located in: `main.c`

```
┌─────────────────────────────────────┐
│     MAIN LOOP (while running)       │
└─────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────┐
│  1. Display Prompt "minishell> "   │
│     (using readline library)        │
└─────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────┐
│  2. Read User Input                 │
│     - readline() waits for input    │
│     - Returns NULL on Ctrl+D (EOF)  │
│     - Handles arrow keys built-in   │
└─────────────────────────────────────┘
                  │
                  ▼
         ┌────────┴────────┐
         │ Input is NULL?  │
         └────────┬────────┘
              YES │    NO
                  │    │
                  │    ▼
                  │  ┌─────────────────────────┐
                  │  │ 3. Check if empty/blank │
                  │  └─────────────────────────┘
                  │              │
                  │              ▼
                  │    ┌──────────────────┐
                  │    │ 4. Add to History│
                  │    │  (add_history()) │
                  │    └──────────────────┘
                  │              │
                  │              ▼
                  │    ┌──────────────────┐
                  │    │ 5. PARSE INPUT   │
                  │    │ (tokenize + cmd) │
                  │    └──────────────────┘
                  │              │
                  │              ▼
                  │    ┌──────────────────┐
                  │    │ 6. EXECUTE CMD   │
                  │    │ (exec_command()) │
                  │    └──────────────────┘
                  │              │
                  │              ▼
                  │    ┌──────────────────┐
                  │    │ 7. Free Memory   │
                  │    │ (gc_free_all())  │
                  │    └──────────────────┘
                  │              │
                  ▼              ▼
         ┌─────────────────────────┐
         │ 8. Check if should exit │
         │    - "exit" command     │
         │    - Ctrl+D (EOF)       │
         └─────────────────────────┘
                  │
                  ▼
         ┌─────────────────┐
         │   Exit Shell    │
         │ (cleanup & exit)│
         └─────────────────┘
```

### Signal Handling During Loop

- **Ctrl+C (SIGINT)**: 
  - Sets `g_sigint_status = 130`
  - Prints newline
  - Shows new prompt (doesn't exit)
  
- **Ctrl+D (EOF)**:
  - readline() returns NULL
  - Prints "exit"
  - Exits shell gracefully

- **Ctrl+\\  (SIGQUIT)**:
  - Ignored in interactive mode

---

## 📝 Parsing Pipeline

### Phase 1: Lexical Analysis (Tokenization)

**Files:** `src/parsing/lexing/tokenize.c`, `tokenize_helper.c`

```
Input: "echo hello | grep h > out.txt"
                  │
                  ▼
┌─────────────────────────────────────┐
│    TOKENIZER (tokenize_input())     │
│                                     │
│  Splits input into tokens based on: │
│  - Spaces (word boundaries)         │
│  - Pipes (|)                        │
│  - Redirections (<, >, <<, >>)      │
│  - Quotes (' and ")                 │
└─────────────────────────────────────┘
                  │
                  ▼
         Token List Created:
         ┌──────────────────┐
         │ TOKEN: "echo"    │
         │ TYPE: WORD       │
         ├──────────────────┤
         │ TOKEN: "hello"   │
         │ TYPE: WORD       │
         ├──────────────────┤
         │ TOKEN: "|"       │
         │ TYPE: PIPE       │
         ├──────────────────┤
         │ TOKEN: "grep"    │
         │ TYPE: WORD       │
         ├──────────────────┤
         │ TOKEN: "h"       │
         │ TYPE: WORD       │
         ├──────────────────┤
         │ TOKEN: ">"       │
         │ TYPE: REDIR_OUT  │
         ├──────────────────┤
         │ TOKEN: "out.txt" │
         │ TYPE: WORD       │
         └──────────────────┘
```

**Token Types:**
- `WORD`: Regular text
- `PIPE`: Pipeline operator `|`
- `REDIR_IN`: Input redirection `<`
- `REDIR_OUT`: Output redirection `>`
- `REDIR_APPEND`: Append redirection `>>`
- `REDIR_HEREDOC`: Heredoc `<<`

### Phase 2: Syntax Validation

**Files:** `src/parsing/lexing/token_check.c`

```
┌─────────────────────────────────────┐
│   check_syntax_errors(tokens)       │
│                                     │
│  Validates:                         │
│  ✓ No empty pipes (| |)            │
│  ✓ No leading/trailing pipes       │
│  ✓ Redirections have targets       │
│  ✓ Quotes are closed               │
│  ✓ Valid operators                 │
└─────────────────────────────────────┘
                  │
         ┌────────┴────────┐
         │   Valid?        │
         └────────┬────────┘
              NO  │  YES
                  │   │
        Print error  │
        Set exit=2   │
        Return       │
                     ▼
```

### Phase 3: Command Building

**Files:** `src/parsing/command/token_to_cmd.c`

```
Token List → Command Structures
                  │
                  ▼
┌─────────────────────────────────────┐
│  build_command_list(tokens)         │
│                                     │
│  For each pipeline segment:         │
│  1. Extract command name & args     │
│  2. Process redirections            │
│  3. Process heredocs                │
│  4. Expand environment variables    │
│  5. Remove quotes                   │
└─────────────────────────────────────┘
                  │
                  ▼
    t_command Linked List:
    
    ┌────────────────────────┐
    │ CMD 1: echo hello      │
    │ args: ["echo", "hello"]│
    │ redir: NULL            │
    │ next: → CMD 2          │
    └────────────────────────┘
              │
              ▼
    ┌────────────────────────┐
    │ CMD 2: grep h          │
    │ args: ["grep", "h"]    │
    │ redir: > out.txt       │
    │ next: NULL             │
    └────────────────────────┘
```

### Phase 4: Environment Variable Expansion

**Files:** `src/parsing/command/expand_env.c`

```
Input: "echo $USER $PWD $?"
                  │
                  ▼
┌─────────────────────────────────────┐
│   expand_env_vars(str)              │
│                                     │
│  Searches for $ patterns:           │
│  - $VAR → lookup in g_shell.env     │
│  - $?   → g_shell.last_status       │
│  - $$   → shell PID                 │
│                                     │
│  Handles quotes:                    │
│  - Double quotes: expand $VAR       │
│  - Single quotes: literal $VAR      │
└─────────────────────────────────────┘
                  │
                  ▼
Output: "echo klejdi /Users/klejdi 0"
```

### Phase 5: Heredoc Processing

**Files:** `src/parsing/command/read_heredoc.c`, `heredoc_utils.c`

```
Input: cat << EOF
       line 1
       line 2
       EOF
                  │
                  ▼
┌─────────────────────────────────────┐
│  exec_heredoc(delimiter, expand)    │
│                                     │
│  1. Create temporary file           │
│  2. Read lines until delimiter      │
│  3. Expand $VAR if not quoted       │
│  4. Write to temp file              │
│  5. Return file descriptor          │
└─────────────────────────────────────┘
                  │
                  ▼
    Temp file: /tmp/heredoc_12345
    Contains: "line 1\nline 2\n"
    FD returned for input redirection
```

---

## ⚙️ Execution Pipeline

### Execution Flow Decision Tree

**Files:** `src/execution/executor_part/exec_basics.c`

```
exec_command(cmd_list)
         │
         ▼
┌────────────────────┐
│ Is it a pipeline?  │
│ (cmd->next != NULL)│
└────────┬───────────┘
         │
    ┌────┴────┐
    NO        YES
    │         │
    ▼         ▼
┌────────┐  ┌──────────────┐
│ Single │  │   Pipeline   │
│ Command│  │   Execution  │
└────────┘  └──────────────┘
    │              │
    ▼              ▼
```

### 1. Single Command Execution

**File:** `src/execution/executor_part/exec_basics.c`

```
┌─────────────────────────────────────┐
│   exec_single_command(cmd)          │
└─────────────────────────────────────┘
                  │
                  ▼
         ┌────────────────┐
         │ Is it builtin? │
         └────────┬───────┘
              NO  │  YES
                  │  │
                  │  ▼
                  │ ┌──────────────────────┐
                  │ │ exec_builtin(cmd)    │
                  │ │ - cd, echo, pwd, etc │
                  │ │ - Runs in parent     │
                  │ │ - Updates g_shell    │
                  │ └──────────────────────┘
                  │
                  ▼
         ┌────────────────────┐
         │ Setup Redirections │
         │ - Open files       │
         │ - Save stdin/out   │
         └────────────────────┘
                  │
                  ▼
         ┌────────────────────┐
         │   fork() process   │
         └────────┬───────────┘
                  │
        ┌─────────┴─────────┐
        │                   │
     PARENT              CHILD
        │                   │
        ▼                   ▼
┌────────────────┐  ┌──────────────────┐
│ wait(status)   │  │ Apply redirects  │
│ Restore FDs    │  │ Find executable  │
│ Update exit    │  │ execve()         │
│ code           │  │ (replaces process│
└────────────────┘  └──────────────────┘
```

### 2. Pipeline Execution

**File:** `src/execution/executor_part/exec_pipeline_helpers.c`

```
Input: "ls -l | grep txt | wc -l"

┌─────────────────────────────────────┐
│   exec_pipeline(cmd_list)           │
│                                     │
│   1. Count commands in pipeline     │
│   2. Create pipes for each link     │
│   3. Fork child for each command    │
│   4. Connect pipes to stdin/stdout  │
│   5. Execute commands in parallel   │
│   6. Wait for all children          │
└─────────────────────────────────────┘

Visual Pipeline Flow:

┌─────────┐    pipe[0]    ┌─────────┐    pipe[1]    ┌─────────┐
│  ls -l  │  ─────────→   │grep txt │  ─────────→   │  wc -l  │
└─────────┘               └─────────┘               └─────────┘
    CMD 1                     CMD 2                     CMD 3
    fork()                   fork()                   fork()
      │                        │                        │
      ├─ dup2(pipe[0][1], 1)  ├─ dup2(pipe[0][0], 0)  ├─ dup2(pipe[1][0], 0)
      │                        ├─ dup2(pipe[1][1], 1)  │
      │                        │                        │
      ▼                        ▼                        ▼
  execve(ls)              execve(grep)              execve(wc)

Parent Process:
  - Closes all pipe ends
  - Waits for all children
  - Returns last command's exit status
```

### 3. Redirection Handling

**Files:** `src/execution/executor_part/exec_redir_*.c`

```
Types of Redirections:

1. INPUT (<)
   ┌──────────────────────┐
   │ cat < input.txt      │
   └──────────────────────┘
            │
            ▼
   fd = open("input.txt", O_RDONLY)
   dup2(fd, STDIN_FILENO)

2. OUTPUT (>)
   ┌──────────────────────┐
   │ echo hi > out.txt    │
   └──────────────────────┘
            │
            ▼
   fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)
   dup2(fd, STDOUT_FILENO)

3. APPEND (>>)
   ┌──────────────────────┐
   │ echo hi >> out.txt   │
   └──────────────────────┘
            │
            ▼
   fd = open("out.txt", O_WRONLY | O_CREAT | O_APPEND, 0644)
   dup2(fd, STDOUT_FILENO)

4. HEREDOC (<<)
   ┌──────────────────────┐
   │ cat << EOF           │
   │ content here         │
   │ EOF                  │
   └──────────────────────┘
            │
            ▼
   fd = exec_heredoc("EOF", expand_vars)
   dup2(fd, STDIN_FILENO)
```

### 4. Built-in Command Execution

**Files:** `src/execution/executor_part/exec_builtins*.c`

```
┌─────────────────────────────────────┐
│         BUILT-IN COMMANDS           │
└─────────────────────────────────────┘

1. cd [directory]
   ┌──────────────────────────┐
   │ Changes current directory│
   │ - Updates PWD and OLDPWD │
   │ - Handles relative/abs   │
   │ - cd with no args → HOME │
   └──────────────────────────┘

2. echo [-n] [args...]
   ┌──────────────────────────┐
   │ Prints arguments         │
   │ - -n: no newline         │
   │ - Expands variables      │
   └──────────────────────────┘

3. pwd
   ┌──────────────────────────┐
   │ Prints working directory │
   │ - getcwd()               │
   └──────────────────────────┘

4. export [VAR=value]
   ┌──────────────────────────┐
   │ Sets environment variable│
   │ - Adds to g_shell.env    │
   │ - Updates existing vars  │
   │ - No args: prints all    │
   └──────────────────────────┘

5. unset [VAR]
   ┌──────────────────────────┐
   │ Removes env variable     │
   │ - Removes from list      │
   └──────────────────────────┘

6. env
   ┌──────────────────────────┐
   │ Prints all env variables │
   │ - Iterates g_shell.env   │
   └──────────────────────────┘

7. exit [n]
   ┌──────────────────────────┐
   │ Exits shell              │
   │ - Optional exit code     │
   │ - Validates numeric      │
   │ - Cleanup & exit         │
   └──────────────────────────┘
```

### 5. External Command Execution

**Files:** `src/execution/executor_part/exec_external_*.c`

```
┌─────────────────────────────────────┐
│  exec_external_command(cmd)         │
└─────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────┐
│ 1. Find executable path             │
│    find_executable(cmd->args[0])    │
│                                     │
│    Search order:                    │
│    a) Absolute path (/bin/ls)       │
│    b) Relative path (./script)      │
│    c) Search $PATH directories      │
└─────────────────────────────────────┘
                  │
                  ▼
         ┌────────────────┐
         │ Found?         │
         └────────┬───────┘
              NO  │  YES
                  │  │
      "command     │
       not found"  │
                   ▼
         ┌────────────────────┐
         │ 2. execve()        │
         │    - path          │
         │    - args          │
         │    - envp          │
         │                    │
         │ Replaces process   │
         └────────────────────┘
```

---

## 🚦 Signal Handling

**File:** `src/execution/executor_part/signals.c`

### Signal Handlers Setup

```c
void setup_signal_handlers(void)
{
    signal(SIGINT, handle_sig_int);    // Ctrl+C
    signal(SIGQUIT, SIG_IGN);          // Ctrl+\ (ignore)
}
```

### Ctrl+C (SIGINT) Handler

```
User presses Ctrl+C
         │
         ▼
┌─────────────────────────────────────┐
│  handle_sig_int(int sig)            │
│                                     │
│  1. g_sigint_status = 130           │
│     (128 + signal number)           │
│                                     │
│  2. write(1, "\n", 1)               │
│     Print newline                   │
│                                     │
│  3. rl_on_new_line()                │
│     Tell readline we're on new line │
│                                     │
│  4. rl_replace_line("", 0)          │
│     Clear current input buffer      │
│                                     │
│  5. rl_redisplay()                  │
│     Show fresh prompt               │
└─────────────────────────────────────┘
         │
         ▼
    Shell continues running
    (doesn't exit!)
```

### Ctrl+D (EOF) Handling

```
User presses Ctrl+D
         │
         ▼
┌─────────────────────────────────────┐
│  readline() returns NULL            │
└─────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────┐
│  Main loop detects NULL             │
│  Prints "exit"                      │
│  Breaks loop                        │
│  Shell exits gracefully             │
└─────────────────────────────────────┘
```

### Signal Behavior Differences

```
┌────────────────────────────────────────────┐
│              Interactive Mode              │
├────────────┬───────────────────────────────┤
│ Ctrl+C     │ New prompt (exit = 130)       │
│ Ctrl+D     │ Exits shell                   │
│ Ctrl+\     │ Ignored                       │
└────────────┴───────────────────────────────┘

┌────────────────────────────────────────────┐
│         Running Command (Child)            │
├────────────┬───────────────────────────────┤
│ Ctrl+C     │ Kills child (exit = 130)      │
│ Ctrl+\     │ Kills child (exit = 131)      │
└────────────┴───────────────────────────────┘
```

---

## 🧹 Memory Management

### Garbage Collector System

**Files:** `Garbage_Collector/garbage_collector*.c`

```
┌─────────────────────────────────────┐
│      Garbage Collector Design       │
│                                     │
│  Centralized memory tracking:       │
│  - All allocations tracked in list  │
│  - Single cleanup point             │
│  - Prevents memory leaks            │
└─────────────────────────────────────┘

Memory Allocation Flow:

    malloc() ────────┐
    strdup() ────────┤
    calloc() ────────┤
                     ▼
         ┌───────────────────┐
         │   gc_malloc()     │
         │   gc_strdup()     │
         │   gc_calloc()     │
         └───────────────────┘
                     │
                     ▼
         ┌───────────────────────┐
         │ Add to gc_list        │
         │ (linked list of ptrs) │
         └───────────────────────┘
                     │
                     ▼
         ┌───────────────────────┐
         │ Return pointer to user│
         └───────────────────────┘


Cleanup Flow (after each command):

         ┌───────────────────┐
         │  gc_free_all()    │
         └───────────────────┘
                  │
                  ▼
         ┌───────────────────┐
         │ Iterate gc_list   │
         │ Free each pointer │
         │ Free list nodes   │
         └───────────────────┘
                  │
                  ▼
         ┌───────────────────┐
         │ Reset gc_list     │
         │ Ready for next    │
         │ command           │
         └───────────────────┘
```

### Memory Lifecycle

```
┌────────────────────────────────────────┐
│           Per Command Cycle            │
└────────────────────────────────────────┘

1. Parse Phase:
   - gc_malloc() for tokens
   - gc_malloc() for command structures
   - gc_strdup() for strings

2. Expansion Phase:
   - gc_malloc() for expanded strings
   - gc_strdup() for env values

3. Execution Phase:
   - Uses allocated structures
   - No additional allocations

4. Cleanup Phase:
   - gc_free_all() called
   - All memory freed
   - Ready for next command

┌────────────────────────────────────────┐
│          Persistent Memory             │
└────────────────────────────────────────┘

Stays allocated across commands:
- g_shell structure
- Environment variable list
- Readline history
```

---

## ✨ Key Features

### 1. Quote Handling

```
Input Types:

1. Single Quotes (')
   echo 'hello $USER'
   → Output: hello $USER
   (NO variable expansion)

2. Double Quotes (")
   echo "hello $USER"
   → Output: hello klejdi
   (Variable expansion happens)

3. Mixed Quotes
   echo 'single' "double" unquoted
   → Processed separately, concatenated

4. Nested Handling
   echo "It's working: $PWD"
   → Quotes removed, $PWD expanded
```

### 2. Variable Expansion

```
Special Variables:

$?  → Last exit status
      echo $?  → "0" or "1" or "130" etc.

$$  → Shell PID
      echo $$  → "12345"

$VAR → Environment variable
       echo $USER → "klejdi"

$EMPTY → Empty variable
         echo $NONEXIST → ""

Expansion Rules:
- Single quotes: NO expansion
- Double quotes: YES expansion
- Unquoted: YES expansion
```

### 3. Exit Status Tracking

```
Status Codes:

0   → Success
1   → General error
2   → Syntax error
126 → Command not executable
127 → Command not found
130 → Terminated by Ctrl+C (SIGINT)
131 → Terminated by Ctrl+\ (SIGQUIT)

Tracking:
- Stored in g_shell.last_status
- Updated after every command
- Accessible via $?
```

### 4. SHLVL (Shell Level)

```
Shell Nesting:

bash (SHLVL=1)
  │
  └─→ minishell (SHLVL=2)
        │
        └─→ minishell (SHLVL=3)

Implementation:
1. On startup: read SHLVL from env
2. Increment by 1
3. Update environment
4. All child processes inherit new SHLVL
```

### 5. Heredoc with Expansion

```
Case 1: Unquoted delimiter (expand)
cat << EOF
Hello $USER
Today is $(date)
EOF

→ Variables expanded
→ Output: "Hello klejdi\nToday is Mon Nov 18..."

Case 2: Quoted delimiter (no expand)
cat << 'EOF'
Hello $USER
EOF

→ Literal output
→ Output: "Hello $USER"
```

---

## 🏗️ Project Structure

```
MinishellMergingPhase/
│
├── main.c                    # Entry point, main loop
├── globals.c                 # Global g_shell definition
├── Makefile                  # Build system
│
├── include/                  # Header files
│   ├── minishell.h          # Main structures
│   ├── parser.h             # Parsing functions
│   ├── executor.h           # Execution functions
│   ├── builtins.h           # Built-in commands
│   ├── garbage_collector.h  # Memory management
│   └── libft/               # Custom library
│
├── src/
│   ├── parsing/
│   │   ├── lexing/          # Tokenization
│   │   │   ├── tokenize.c
│   │   │   ├── token_check.c
│   │   │   └── token_utils.c
│   │   │
│   │   └── command/         # Command building
│   │       ├── token_to_cmd.c
│   │       ├── expand_env.c
│   │       ├── heredoc_utils.c
│   │       └── read_heredoc.c
│   │
│   └── execution/
│       └── executor_part/
│           ├── exec_basics.c         # Main execution
│           ├── exec_builtins*.c      # Built-in commands
│           ├── exec_external_*.c     # External commands
│           ├── exec_pipeline_*.c     # Pipeline handling
│           ├── exec_redir_*.c        # Redirections
│           ├── signals.c             # Signal handlers
│           └── init_shell.c          # Initialization
│
└── Garbage_Collector/       # Memory management system
    ├── garbage_collector.c
    └── garbage_collector_utils.c
```

---

## 🎓 Evaluation Talking Points

### Architecture Highlights

1. **Separation of Concerns**
   - Parsing completely separate from execution
   - Each module has clear responsibility
   - Easy to debug and maintain

2. **Memory Safety**
   - Custom garbage collector prevents leaks
   - All allocations tracked centrally
   - Cleanup after each command

3. **Signal Handling**
   - Proper Ctrl+C handling without exit
   - Readline integration for smooth UX
   - Different behavior for interactive vs child

4. **Error Handling**
   - Syntax errors caught early (parsing)
   - Execution errors handled gracefully
   - Proper exit status codes

### Feature Completeness

✅ **Mandatory Features:**
- Prompt display
- Command history
- Command execution (absolute, relative, PATH)
- Quote handling (single and double)
- Redirections (<, >, <<, >>)
- Pipes (|)
- Environment variables ($VAR, $?)
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+\)
- Built-ins: echo, cd, pwd, export, unset, env, exit

✅ **Bonus Features:**
- Multiple pipes (unlimited)
- Heredoc with expansion control
- SHLVL tracking
- Advanced quote parsing
- Comprehensive error messages

### Code Quality

- **Norminette compliant** (42 coding standard)
- **No memory leaks** (valgrind clean)
- **Modular design** (easy to extend)
- **Well documented** (clear function names)
- **Error resistant** (handles edge cases)

---

## 🔍 Common Test Cases

### Basic Commands
```bash
minishell> ls -la
minishell> echo hello world
minishell> pwd
minishell> cd /tmp
```

### Quotes
```bash
minishell> echo "hello $USER"
minishell> echo 'hello $USER'
minishell> echo "mixed 'quotes' test"
```

### Redirections
```bash
minishell> echo test > file.txt
minishell> cat < file.txt
minishell> echo append >> file.txt
minishell> cat << EOF
```

### Pipes
```bash
minishell> ls | grep txt
minishell> ls -l | grep txt | wc -l
minishell> echo hello | cat | cat | cat
```

### Environment
```bash
minishell> export MY_VAR=hello
minishell> echo $MY_VAR
minishell> unset MY_VAR
minishell> env
```

### Exit Status
```bash
minishell> ls
minishell> echo $?        # Should print 0
minishell> ls /nonexist
minishell> echo $?        # Should print 1 or 2
```

### Signals
```bash
minishell> cat
^C                        # Ctrl+C (new prompt)
minishell> cat
^D                        # Ctrl+D (exits cat)
minishell> ^D             # Ctrl+D (exits shell)
```

---

## 🎯 Quick Summary for Evaluation

**What is Minishell?**
A simplified bash-like shell implementing core shell functionality: parsing, execution, pipes, redirections, and built-in commands.

**How does it work?**
1. **Read** user input with readline
2. **Tokenize** into words and operators
3. **Parse** into command structures
4. **Expand** environment variables
5. **Execute** with fork/exec or built-ins
6. **Handle** signals and cleanup memory

**Key Technical Decisions:**
- Garbage collector for memory safety
- Token-based parser for flexibility
- Pipeline execution with fork/pipe
- Readline for enhanced UX
- Global shell state for easy access

**Challenges Overcome:**
- Heredoc implementation with expansion
- Quote parsing and removal
- Pipeline file descriptor management
- Signal handling with readline
- Memory leak prevention

---

**Good luck with your evaluation! 🚀**

This minishell demonstrates solid understanding of:
- Process management (fork, exec, wait)
- File descriptors and I/O redirection
- Inter-process communication (pipes)
- Signal handling
- Memory management
- String parsing and manipulation
- Unix system programming
