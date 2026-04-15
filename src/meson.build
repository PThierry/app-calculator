# SPDX-License-Identifier: Apache-2.0
#
# Copyright 2025 Ledger SAS

calculator_inc = include_directories('.')

calculator_lib = static_library('calculator',
  'calculator.c',
  include_directories: [ calculator_inc ],
)

calculator_dep = declare_dependency(
  link_with: calculator_lib,
  include_directories: [ calculator_inc ],
)

calculator_src += files(
  'main.c',
  'console.c',
  'stm32_usart_driver.c',
)
