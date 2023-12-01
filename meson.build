project(
  'mehawk',
  [ 'c', 'cpp' ],
  version : '0.1.0',
  meson_version : '>= 0.63.0',
  license: 'mit'
)

# have to do that to avoid MSVC breaking code with their min/max macros
if(meson.get_compiler('cpp').get_id() == 'msvc')
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

if(meson.get_compiler('cpp').get_id() == 'clang')
  add_project_arguments(['-stdlib=libc++'], language: 'cpp')
  add_project_arguments('-D_LIBCPP_DEBUG', language: 'cpp')
endif

if(meson.get_compiler('cpp').get_id() == 'gcc')
  add_project_arguments('-DGLIBCXX_DEBUG', language: 'cpp')
endif


cmake = import('cmake')

build_type_opts = cmake.subproject_options()
build_type_opts.add_cmake_defines({ 'CMAKE_BUILD_TYPE': get_option('buildtype').to_upper() } )

libassert = dependency('libassert', method: 'auto', required: false)
if not libassert.found()
  libassert_sub = cmake.subproject('libassert', options: build_type_opts, required: true)
  libassert = libassert_sub.dependency('assert')
endif

optional = dependency('optional', method: 'auto', required: false)
if not optional.found()
  optional_sub = cmake.subproject('optional', options: build_type_opts, required: true)
  optional = optional_sub.dependency('optional')
endif

expected = dependency('expected', method: 'auto', required: false)
if not expected.found()
  expected_sub = cmake.subproject('expected', options: build_type_opts, required: true)
  expected = expected_sub.dependency('expected')
endif

fmt = dependency('fmt', method: 'auto', required: false)
if not fmt.found()
  fmt_sub = cmake.subproject('fmt', options: build_type_opts, required: true)
  fmt = fmt_sub.dependency('fmt')
endif

toml11 = dependency('toml11', method: 'auto', required: false)
if not toml11.found()
  toml11_sub = cmake.subproject('toml11', options: build_type_opts, required: true)
  toml11 = toml11_sub.dependency('toml11')
endif

local_deps = [
  dependency('threads', required: true),

  libassert,
  optional,
  expected,
  toml11,
  fmt,

  dependency('magic_enum'),
  dependency('spdlog'),
]

mehawk_compilation_options = [
  'cpp_std=c++20'
]

include_dirs = [
  include_directories('src')
]

sources = files(
  'src/mehawk/standard_paths.cpp',
  'src/mehawk/env.cpp',
  'src/mehawk/app.cpp'
)

mehawk_dep = declare_dependency(
  sources: sources,
  dependencies: local_deps,
  include_directories: include_dirs,
)

subdir('src')
subdir('test')
