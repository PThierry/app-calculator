# SPDX-License-Identifier: Apache-2.0

calculator_native_lib = static_library('calculator_native',
  '../src/calculator.c',
  include_directories: include_directories('../src'),
  native: true,
)

calculator_native_dep = declare_dependency(
  link_with: calculator_native_lib,
  include_directories: include_directories('../src'),
)

executable('calculator_cli',
  'calculator_cli.c',
  dependencies: [ calculator_native_dep ],
  native: true,
  install: false,
)
