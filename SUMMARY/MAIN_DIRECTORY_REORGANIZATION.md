# Main Directory Reorganization - 42 Norm Compliant

## Overview
Successfully reorganized `main.c` (580+ lines) into a clean `main/` directory structure with **9 modular files**, each following the 42 norm:
- ✅ Maximum 5 functions per file (achieved)
- ✅ Maximum 25 lines per function (achieved)
- ✅ Clean separation of concerns
- ✅ All functions properly documented

## Directory Structure

```
main/
├── main.h                      # Header with all prototypes
├── main.c                      # Entry point (main function)
├── main_loop.c                 # Main shell loop
├── input_processing.c          # Input/command processing
├── env_utils.c                 # Environment utility functions
├── env_merge.c                 # Environment merging logic
├── single_cmd.c                # Single command execution
├── single_cmd_setup.c          # Command redirection setup
├── single_cmd_env.c            # Environment assignment handling
└── pipeline.c                  # Pipeline execution

Total: 9 C files + 1 header (702 lines total)
```

## File Breakdown

### 1. `main.c` (Entry Point)
**Functions: 1 public + 2 static**
- `main()` - Program entry point
- `increment_shlvl()` - Increments SHLVL environment variable
- `init_environment()` - Initializes environment from envp

### 2. `main_loop.c` (Shell Loop)
**Functions: 1 public + 2 static**
- `main_loop()` - Main shell execution loop
- `handle_eof()` - Handles EOF (Ctrl+D) scenarios
- `handle_sigint()` - Handles SIGINT (Ctrl+C) signals

### 3. `input_processing.c` (Input Handling)
**Functions: 3 public**
- `process_command()` - Routes command to single/pipeline execution
- `process_input_line()` - Tokenizes and processes input line
- `read_line_noninteractive()` - Reads input in non-interactive mode

### 4. `env_utils.c` (Environment Utilities)
**Functions: 2 public + 1 static**
- `env_list_to_array()` - Converts environment list to array
- `cmdlist_to_array()` - Converts command list to array
- `create_env_string()` - Creates KEY=VALUE string

### 5. `env_merge.c` (Environment Merging)
**Functions: 1 public + 3 static**
- `merge_env_arrays()` - Merges base and per-command environments
- `allocate_merged_array()` - Allocates merged array
- `copy_base_env()` - Copies base environment
- `add_cmd_env()` - Adds command-specific environment

### 6. `single_cmd.c` (Single Command Execution)
**Functions: 1 public + 3 static**
- `handle_single_command()` - Executes single command
- `is_empty_command()` - Checks if command is empty
- `execute_builtin()` - Executes builtin commands
- `execute_external()` - Executes external commands

### 7. `single_cmd_setup.c` (Redirection Setup)
**Functions: 4 public + 1 static**
- `setup_cmd_redirections()` - Sets up input/output redirections
- `apply_cmd_redirections()` - Applies redirections to file descriptors
- `restore_cmd_fds()` - Restores original file descriptors
- `cleanup_cmd_redir_failure()` - Cleans up on redirection failure
- `save_std_fds()` - Saves stdin/stdout file descriptors

### 8. `single_cmd_env.c` (Assignment Handling)
**Functions: 1 public + 4 static**
- `handle_assignment_only()` - Handles variable assignments without commands
- `find_existing_env()` - Finds existing environment variable
- `update_existing_env()` - Updates existing environment variable
- `add_new_env()` - Adds new environment variable
- `set_env_var()` - Sets environment variable in shell

### 9. `pipeline.c` (Pipeline Execution)
**Functions: 1 public + 2 static**
- `handle_pipeline()` - Executes command pipeline
- `build_per_cmd_envs()` - Builds per-command environment arrays
- `cleanup_per_cmd_envs()` - Cleans up environment arrays

## 42 Norm Compliance

### ✅ Function Count
- All files have ≤ 5 functions
- Average: ~3 functions per file
- Distribution is well-balanced

### ✅ Function Length
- All functions are ≤ 25 lines
- Most functions are 15-20 lines
- Complex logic is properly split

### ✅ File Organization
- Clear logical separation
- Single responsibility per file
- Minimal coupling between files

### ✅ Naming Conventions
- All functions use snake_case
- Descriptive names (self-documenting)
- Consistent prefix patterns

## Build Integration

### Makefile Changes
```makefile
INCLUDES = -Iinclude -Iinclude/libft -Isrc/execution/executor_part -IGarbage_Collector -Imain

MAIN_SRCS = \
    main/main.c \
    main/main_loop.c \
    main/input_processing.c \
    main/env_utils.c \
    main/env_merge.c \
    main/single_cmd_setup.c \
    main/single_cmd_env.c \
    main/single_cmd.c \
    main/pipeline.c

MAIN_OBJS = $(MAIN_SRCS:.c=.o)

OBJS = $(MAIN_OBJS) $(GLOBAL_OBJ) $(PARSER_OBJS)
```

### Header File
- `main/main.h` contains all prototypes
- Included in all main/*.c files
- Clear documentation of dependencies

## Testing

### Build Test
```bash
✅ make fclean && make
✅ All files compile without warnings
✅ Linking successful
✅ Binary created: minishell
```

### Functionality Test
```bash
✅ Basic commands work
✅ Environment variables work
✅ Pipes work
✅ Redirections work
✅ All features preserved
```

## Backup

**Original file preserved**: `main.c.backup`
- Can be restored if needed
- Contains original 580+ line monolithic code

## Benefits

### 1. **Maintainability**
- Easy to locate specific functionality
- Changes isolated to relevant files
- Clear function responsibilities

### 2. **42 Norm Compliance**
- All functions ≤ 25 lines
- All files have ≤ 5 functions
- Proper header usage

### 3. **Readability**
- Logical file names
- Clear function names
- Well-organized structure

### 4. **Team Collaboration**
- Multiple people can work on different files
- Reduced merge conflicts
- Clear ownership of functionality

## Summary

| Metric | Before | After | Status |
|--------|--------|-------|--------|
| Files | 1 | 9 | ✅ |
| Lines/File | 580+ | ~78 avg | ✅ |
| Functions/File | 15+ | ~3 avg | ✅ |
| Max Function Lines | 100+ | 25 | ✅ |
| 42 Norm Compliant | ❌ | ✅ | ✅ |

**Result**: Clean, modular, 42-norm-compliant code structure! 🎉

---
*Reorganized: November 18, 2025*
