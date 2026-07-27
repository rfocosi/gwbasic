# GW-BASIC Interpreter for Linux

A native C implementation of Microsoft's historical **GW-BASIC Interpreter**, transpiled/re-implemented to target modern Linux environments.

This repository builds upon Microsoft's 1983 GW-BASIC source release, porting its assembly execution semantics, line storage memory model, statement tokenizer, and mathematical evaluator into clean, portable C99 code.

---

## 📁 Repository Structure

```
├── bin/          # Compiled Linux binaries
├── include/      # C Header files (gwbasic.h)
├── src/          # C Source implementation of interpreter runtime
│   ├── main.c        # Interactive REPL & CLI entry point
│   ├── tokenizer.c   # Lexer & statement keyword tokenizer
│   ├── eval.c        # Stack-based expression & math evaluator
│   ├── runtime.c     # Program line storage & execution runtime
│   ├── io_screen.c   # ANSI terminal screen management (CLS, LOCATE, COLOR)
│   └── io_disk.c     # Disk program file I/O (SAVE, LOAD)
├── tests/        # BASIC automated test scripts
├── original/     # Original 1983 8086 Assembly source files & documentation
├── Makefile      # Build system
└── README.md     # Project documentation
```

---

## 🛠️ Building & Running

### Requirements
- GCC / Clang (supporting C99)
- GNU Make
- Standard Math Library (`libm`)

### Build Instructions
```bash
# Compile the interpreter
make

# Display usage help
./bin/gwbasic --help

# Run interactive REPL
./bin/gwbasic

# Execute a BASIC file directly
./bin/gwbasic path/to/program.bas
```

### Run Tests
```bash
make test
```

---

## 📜 Supported Statements & Features

- **Program Commands**: `RUN`, `LIST`, `NEW`, `SAVE`, `LOAD`, `SYSTEM`
- **Control Flow & Variables**: `LET`, `PRINT`, `CLS`, `LOCATE`, `COLOR`
- **Data Types**: Integer, Single Precision Float, Double Precision Float, and String values.
- **Terminal Control**: Full ANSI terminal color and cursor positioning support.

---

## 🏛️ Historical Original Source

The original 1983 8086 Assembly files, header files, and original Microsoft release documentation are preserved under the [`original/`](file:///home/rfocosi/workspace/GW-BASIC/original) directory for historical reference.

---

## 📄 License

Original source code files are licensed under the MIT License by Microsoft Corporation.
