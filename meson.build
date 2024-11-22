project('cpj', 'c',
  license: 'MIT',
  version: '1.2.9',
  meson_version: '>= 0.57.0'
)

cpj_inc = include_directories('include')

cpj_c_args = []
if get_option('default_library') == 'shared'
  cpj_c_args += '-DCPJ_SHARED'
endif

cpj = library('cpj', 'src/cpj.c',
  install: true,
  include_directories: cpj_inc,
  c_args: cpj_c_args
)

install_headers('include/cpj.h')

cpj_dep = declare_dependency(include_directories: 'include', link_with: cpj)

if get_option('ENABLE_TESTS')
  subdir('test')
endif

pkg = import('pkgconfig')
pkg.generate(cpj)
