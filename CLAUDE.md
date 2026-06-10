# darp — Derek's Array Programming Language

Single-file C project (`main.c`). No build system yet; compile with:

    cc -Wall -Wextra -o darp main.c

## Status

Working REPL loop with tokenizer. `eval()` is currently a debug stub that
prints token info; no parser or evaluator yet.

## Architecture

Everything lives in `main.c` for now. Split into separate files once the
parser starts to grow.

### REPL (`main`)

- `buf[BUF_SIZE]` — global input buffer (4096 bytes).
- Prompts `"   "` on a fresh expression, `".. "` when continuing.
- `fgets` appends into `buf` at `strlen(buf)` so multi-line input accumulates.
- Calls `eval()` after each line.
  - `0` → success, clear buf.
  - `-1` → error, clear buf.
  - `1` → incomplete, leave buf and read another line (parser not yet returning this).
- Loop exits when `fDone` is set (or EOF).

### Tokenizer (`next`)

Global state: `pos` (next char index), `mark` (token start), `t` (type),
`tn` (int value for numbers).

Token types via `t`:
- `'A'` — text: starts with a letter, continues with `[a-zA-Z0-9_]`,
  trailing underscores are silently trimmed (left in buffer as symbols).
- `'1'` — number: non-negative decimal integer fitting in `int`.
- any other printable char — single-character symbol.
- `0` — EOF.

Returns `0` on success, `-1` on error (int overflow, non-printable byte).

### `eval()`

Resets `pos = 0` and loops `next()` until EOF or error. Currently just
prints token debug output. This is where the parser goes next.

## Grammar (from grammar.txt)

```
Num  = [0-9]+
Vsym = [A-Z][a-zA-Z0-9_]*
Verb = Vsym
     | '{' Expr (';' Expr)* '}'
     | Vsym ':' Verb
Noun = Num
     | [a-z][a-zA-Z0-9_]*
     | '[' Noun+ ']'
     | '(' ',' Noun* ')'

Expr = Expr [ Verb [ Adv ] Expr ]
     | Verb [ [ Adv ] Expr ]
     | '(' Expr ')'
```

Open questions noted by the author:
- Is `f(1)` the same as `f 1`? Could work for named args: `f{y:1}`.
- Dict syntax still todo.

## What to do next

- Build a recursive-descent parser in `eval()` following the grammar above.
- Return `1` (incomplete) from `eval()` when the parser hits EOF mid-expression
  (e.g. open brace or bracket with no close) so the REPL prompts for more input.
- Add a value representation (array/scalar) and a simple evaluator.
- Add a Makefile once there are multiple source files.

## Style notes

- C99, no external dependencies.
- Globals are fine for interpreter state (pos, mark, t, tn, buf, fDone).
- No comments explaining *what* code does — only *why* when non-obvious.
- Keep functions short; split files when a logical boundary appears.
