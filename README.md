# Calculator App

This project is a small embedded calculator application built with Meson.

It provides:
- A UART console prompt (`> `)
- Parsing and evaluation of arithmetic expressions
- Operator precedence and parentheses support (`+`, `-`, `*`, `/`, `(...)`)
- 32-bit unsigned integer arithmetic with overflow checks
- Integer division with remainder reporting

The firmware entry point is in `src/main.c`. The calculator parser/evaluator is implemented in `src/calculator.c`.

## Expression behavior

- Spaces are ignored in expressions.
- Only unsigned integer numbers are accepted.
- Invalid syntax, overflow, divide-by-zero, or negative subtraction results are rejected.
- On success, output format is:
  - `Resultat: <value>`
  - `Resultat: <value> (reste : <remainder>)` for divisions
- On error, output is `Erreur`.

Note: output messages are currently in French because they come from the application source.

## Build prerequisites

- Meson (>= 1.4.0)
- Ninja
- `pkg-config`
- Toolchain and Camelot SDK dependencies required by your platform (for cross build)

## Build the embedded firmware

From the project root :

```bash
export PKG_CONFIG_PATH=${CAMELOT_SDK_ROOT}/lib/pkgconfig
```

```bash
meson setup \
  -Dconfig=configs/sample.config \
  --cross-file cm33-none-eabi-gcc.ini \
  -Ddts=dts/sample.dts \
  -Ddts-include-dirs=${CAMELOT_SDK_ROOT}/share/dts \
  builddir

meson compile -C builddir
```

The produced firmware ELF is:

- `builddir/calculator_app.elf`

## Build the native CLI calculator (host)

A host-side CLI binary is defined in `tests/calculator_cli.c` for quick validation of expression parsing.

```bash
meson setup builddir_host
meson compile -C builddir_host calculator_cli
```

Binary path:

- `builddir_host/tests/calculator_cli`

Example:

```bash
echo "(8 + 4) / 3" | ./builddir_host/tests/calculator_cli
```

Expected output:

```text
Resultat: 4 (reste : 0)
```

## Project layout

- `src/main.c`: firmware main loop, UART init, console interaction
- `src/console.c`: line input and prompt handling over UART
- `src/calculator.c`: expression parser and evaluator
- `src/stm32_usart_driver.c`: basic USART driver that rely on Camelot-OS merlin driver framework
- `tests/calculator_cli.c`: native test/validation CLI executable
- `meson.build`: root build definition
