# meBash

Creating my own Bash using C.

## Main Objective

Recreate the Bash shell.

## Plan of Action

### Phase 1 — Build the shell skeleton

Work through the core pipeline in order:

1. **Take input** — read a line of text from the user.
2. **Lexer** — split the input text into tokens (words, pipes, redirections, operators, etc.).
3. **Expansion** — expand the tokens (variables, quoting, globbing, etc.).
4. **Parser** — turn the expanded tokens into structured data the shell understands.
5. **Execution** — use the parsed data to run processes and subprocesses.

The goal of this phase is to get a minimal but complete path from typed input to an executed command.

### Phase 2 — Add shell features

Once the skeleton works end to end:

- Implement **builtin functions**.
- Implement other shell-level commands and utilities such as `cd`, `list`, etc.

## Current Progress

| Stage | Status | Notes |
| --- | --- | --- |
| Take input | ✅ Working | Reads a line from stdin into a growable buffer |
| Lexer | 🟡 Mostly working | Tokenizes words and shell operators; no quote removal yet |
| Expansion | ⬜ Not started | `expand.c` / `expand.h` are empty |
| Parser | ⬜ Not started | `parser.c` / `parser.h` are empty |
| Execution | ⬜ Not started | The shell only lexes and prints tokens |
| Builtins | ⬜ Not started | No builtins implemented |

Phase 1 is roughly **one third complete**: input works, the lexer is functional, and the remaining three stages are still to be built.

### 1. Take input — ✅ Implemented

What works today:

- `get_user_input_text()` reads from `stdin` one character at a time until `\n` or `EOF`.
- The buffer starts at `1024` bytes (`INPUT_BUFFER_SIZE`) and grows with `realloc` in `1024`-byte steps.
- Returns `NULL` when `EOF` is hit at the start of a line, which ends the shell loop.
- Blank lines are returned as an empty string and skipped by the shell loop.
- The prompt (`> `) is printed and flushed by the main loop before each read.
- Allocation failures are reported and terminate the program.

Not supported yet:

- Line editing, history, or cursor movement (no `readline`).
- Multi-line input / continuation prompts.
- Reading from script files or `-c` arguments.

### 2. Lexer — 🟡 Implemented, needs refinement

What works today:

- Splits input into words and operators.
- Recognizes these operators: `|`, `||`, `&&`, `&`, `;`, `<`, `<<`, `>`, `>>`.
- Matches two-character operators before one-character ones (longest match / maximal munch).
- Skips whitespace between tokens.
- Tokens store their type, heap-allocated value, and starting position.
- Handles single and double quotes while scanning a word, including escaped quotes inside double quotes.
- Reports unterminated quotes with the quote character and its position, and frees any partial token list.
- Backslash escapes stop the next character from ending or splitting a word.
- Token list grows dynamically (doubling from an initial capacity of `32`).
- Provides `free_tokens()` and `print_tokens()` helpers.

Not supported yet:

- Quote removal: quotes and backslashes are kept inside the token value.
- Variable expansion (`$VAR`), command substitution (`$(...)`), or arithmetic expansion.
- Comments (`#`) and heredoc bodies (only the `<<` operator is recognized).
- Newline tokens and operator placement validation (for example, a leading `|` is currently accepted as a token).
- Context-aware quote handling after lexing; the lexer only scans over quoted sections.

### 3. Expansion — ⬜ Not started

`src/text_process_pipeline/expand.c` and `include/text_process_pipeline/expand.h` are empty.

Planned support:

- Variable expansion (`$VAR`, `${VAR}`).
- Tilde expansion (`~`).
- Quote removal.
- Word splitting.
- Pathname expansion / globbing (`*`, `?`, `[...]`).
- Command substitution and arithmetic expansion.

### 4. Parser — ⬜ Not started

`src/text_process_pipeline/parser.c` and `include/text_process_pipeline/parser.h` are empty.

Planned support:

- Grammar for simple commands, pipelines, lists, and background jobs.
- Redirection attachment to commands.
- Validation of operator placement and syntax errors.
- An AST or command structure that execution can consume.

Right now the main loop uses the raw `TokenList` directly; there is no syntax tree.

### 5. Execution — ⬜ Not started

The shell currently only lexes the input and prints the tokens:

```sh
> echo hello | cat
WORD      echo             (pos 0)
PIPE      |                (pos 5)
WORD      cat              (pos 7)
```

Planned support:

- Command lookup and `execvp`-style process launching.
- Pipes and I/O redirection.
- Background jobs and job control.
- Waiting for children and reporting exit statuses.
- Signal handling.

## Phase 2 — Shell Features

Not started yet.

Planned builtins and shell-level features:

- Basic builtins such as `cd`, `exit`, `export`, `unset`, and `pwd`.
- Shell-level commands and utilities such as `list`.
- External command execution through `PATH` once the execution stage exists.

> Note: `cd` must be a builtin because changing the current directory only affects the shell process itself.

## Project Structure

```
src/
├── meBash.c                     # Entry point and shell loop
└── text_process_pipeline/
    ├── input.c                  # Read user input (implemented)
    ├── lexer.c                  # Tokenize input (implemented)
    ├── expand.c                 # Expand tokens (empty)
    └── parser.c                 # Parse tokens (empty)
include/
└── text_process_pipeline/
    ├── input.h                  # Implemented
    ├── lexer.h                  # Implemented
    ├── expand.h                 # Empty
    └── parser.h                 # Empty
```

## Build

```sh
cmake -S . -B build
cmake --build build
```
