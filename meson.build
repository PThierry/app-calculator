# SPDX-License-Identifier: Apache-2.0
#
# Copyright 2025 Ledger SAS
# Copyright 2026 H2Lab Development Team

project('sample-c-app', 'c',
    meson_version: '>=1.4.0',
    version: '0.0.1',
    default_options: [
        'c_std=gnu11',
        'warning_level=3', 'optimization=s',
        'default_library=static', 'b_pie=false', 'b_staticpic=false',
        'dts=dts/sample.dts',
        'config=configs/sample.config',
    ],
)

calculator_deps = []

camelot_dts_include_dir = meson.get_external_property(
    'camelot_dts_include_dir',
    meson.current_source_dir() / 'dts',
)

target_cflags = run_command(
    'pkg-config', '--cflags', 'shield',
    capture: true,
    check: true,
    env: [ 'PKG_CONFIG_PATH=@0@'.format(get_option('pkg_config_path')[0]) ],
).stdout().strip().split()

add_global_arguments(target_cflags, language: [ 'c' ])

kconfig_file = meson.current_source_dir() / 'Kconfig'
kconfig_proj = subproject('kconfig', default_options: ['kconfig=@0@'.format(kconfig_file)])
kconfig_h = kconfig_proj.get_variable('kconfig_h')
kconfig_data = kconfig_proj.get_variable('kconfig_data')

package_metadata_proj = subproject('package-metadata')
package_metadata_dep = package_metadata_proj.get_variable('package_metadata_dep')
calculator_deps += [ package_metadata_dep ]

merlin_proj = subproject('merlin',
    default_options: [
        'dts=' + meson.current_source_dir() / 'dts/sample.dts',
        'dts-include-dirs=' + camelot_dts_include_dir,
    ]
)
merlin_lib = merlin_proj.get_variable('merlin_lib')
merlin_dep = merlin_proj.get_variable('merlin_dep')
calculator_deps += [ merlin_dep ]

summary(
    {
        'dts include dir': camelot_dts_include_dir,
    },
    bool_yn: true,
    section: 'configuration',
)

calculator_src = files()
subdir('src')
calculator_deps += [ calculator_dep ]
subdir('tests')

calculator_elf = executable(
    meson.project_name(),
    name_suffix: 'elf',
    sources: calculator_src,
    dependencies: [ calculator_deps ],
    link_language: 'c',
    install: true,
    c_args: '-DWITH_STDLIB',
)
