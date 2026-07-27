# GW-BASIC (Linux Port) User Manual

This manual describes the syntax, command line usage, and supported basic commands for the modern Linux C port of the historical **Microsoft GW-BASIC Interpreter**.

---

## 1. Command Line Usage

The `gwbasic` binary supports the following execution modes:

### A. Display Help Information
Display command line usage and flags using `-h`, `--help`, or `-?`:
```bash
./bin/gwbasic -h
```

### B. Interactive REPL Mode
Run `gwbasic` without arguments to launch the interactive prompt:
```bash
./bin/gwbasic
```
**Terminal Output:**
```text
GW-BASIC 3.23 (Linux Port)
(C) Copyright Microsoft 1983, 1984, 1985, 1986, 1987, 1988.
60000 Bytes free
Ok
```
In interactive mode, commands can be entered directly or stored as numbered program lines. Type `SYSTEM` or press `Ctrl+D` to exit.

### C. Direct File Execution Mode
Run a BASIC source script directly from the Linux shell:
```bash
./bin/gwbasic path/to/program.bas
```
The interpreter will load the file, execute all stored program statements, and exit back to the Linux shell upon completion.

### D. Test Execution Mode
Run a test script using the `--test` flag:
```bash
./bin/gwbasic --test tests/run_all.bas
```
In test mode, errors during file loading return exit code `1`.

---

## 2. Program Editing & Line Storage

GW-BASIC operates using two line execution modes:

- **Direct Execution Mode (No Line Number)**: Statements entered without a line number are executed immediately when you press Enter.
- **Program Storage Mode (Line Number Prefixed)**: Statements starting with an integer line number (1 to 65529) are stored into memory in sorted numerical order.

### Line Management Operations:
- **Add / Replace Line**: Type a line number followed by BASIC statements.
  ```basic
  10 PRINT "Hello World"
  ```
- **Delete Line**: Type the line number alone and press Enter.
  ```basic
  10
  ```

---

## 3. Supported Data Types & Expressions

The evaluator supports the following data types and operations:

| Type | Syntax / Variable Suffix | Example Values |
|---|---|---|
| **Integer** | Plain identifier (e.g. `A`, `COUNT`) | `10`, `-42` |
| **Single-Precision Float** | Decimal numbers | `3.14159`, `0.5` |
| **Double-Precision Float** | Double-precision floats | `123.456789` |
| **String** | String variable ending in `$` or literals enclosed in `""` | `"Hello"`, `NAME$` |

### Arithmetic Operators
- `+` Addition (and string concatenation)
- `-` Subtraction
- `*` Multiplication
- `/` Division
- `%` Modulo (integer remainder)
- `(` and `)` Parentheses for grouping expressions

---

## 4. Basic Commands Reference

Below are the commands supported by the interpreter runtime:

### Environment & Program Management Commands

#### `LIST`
Displays all statements currently loaded in the program buffer, sorted by line number.
```basic
LIST
```

#### `RUN`
Executes the stored BASIC program starting from the lowest line number.
```basic
RUN
```

#### `NEW`
Clears the current program memory buffer and resets internal interpreter state.
```basic
NEW
```

#### `SAVE`
Saves the currently loaded program lines to a text file on disk.
```basic
SAVE "myprog.bas"
```
*(Or without quotes: `SAVE myprog.bas`)*

#### `LOAD`
Clears existing memory and loads program lines from a specified text file on disk.
```basic
LOAD "myprog.bas"
```

#### `SYSTEM`
Exits the GW-BASIC interpreter environment and returns to the operating system prompt.
```basic
SYSTEM
```

---

### Variable Assignment & Control Statements

#### `LET` / Variable Assignment
Assigns a value or expression to a variable name. The `LET` keyword is optional.
```basic
10 LET A = 100
20 B = 200
30 MSG$ = "Result = "
```

---

### Input / Output & Terminal Screen Commands

#### `PRINT` (Shorthand: `?`)
Outputs text, numbers, or variable values to the console.
- Use commas `,` to separate values with tab stops (`\t`).
- Use semicolons `;` to concatenate values without spaces.
- A trailing semicolon `;` suppresses the default newline.

```basic
10 LET X = 15
20 LET Y = 25
30 PRINT "X = "; X, "Y = "; Y
40 ? "Sum is: "; X + Y
```
**Output:**
```text
X = 15 	Y = 25
Sum is: 40
```

#### `CLS`
Clears the active terminal screen using ANSI escape sequences.
```basic
10 CLS
```

#### `LOCATE`
Positions the terminal cursor at a specific `row, col` location (1-indexed).
```basic
10 LOCATE 5, 10
20 PRINT "Centered Text"
```

#### `COLOR`
Sets the terminal foreground (`fg`) and background (`bg`) colors using standard ANSI terminal attributes.
```basic
10 COLOR 2, 0
20 PRINT "Green text on black background"
```

---

## 5. End-to-End Example Session

Here is an example of an interactive session creating, saving, running, and managing a BASIC program:

```basic
GW-BASIC 3.23 (Linux Port)
(C) Copyright Microsoft 1983, 1984, 1985, 1986, 1987, 1988.
60000 Bytes free
Ok
10 CLS
Ok
20 COLOR 3, 0
Ok
30 LET A = 12
Ok
40 LET B = 8
Ok
50 PRINT "Calculated Value: "; A * B + 4
Ok
LIST
10 CLS
20 COLOR 3, 0
30 LET A = 12
40 LET B = 8
50 PRINT "Calculated Value: "; A * B + 4
Ok
RUN
Calculated Value: 100
Ok
SAVE "calc.bas"
Ok
NEW
Ok
LIST
Ok
LOAD "calc.bas"
Ok
RUN
Calculated Value: 100
Ok
SYSTEM
```

