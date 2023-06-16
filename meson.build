project(
  'haumea',
  'cpp',
  version : '0.1.0',
  meson_version : '>= 0.63.0',
  license: 'mit'
)

# have to do that to avoid MSVC breaking code with their min/max macros
if(meson.get_compiler('cpp').get_id() == 'msvc')
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

if(meson.get_compiler('cpp').get_id() == 'clang')
  add_project_arguments(['-stdlib=libc++'], language : 'cpp')
endif

cmake = import('cmake')

build_type_opts = cmake.subproject_options()
build_type_opts.add_cmake_defines( { 'CMAKE_BUILD_TYPE': get_option('buildtype').to_upper() } )

dpp_sub = cmake.subproject('dpp', options: build_type_opts, required: true)
libassert_sub = cmake.subproject('libassert', options: build_type_opts, required: true)

local_deps = [
  dependency('threads', required: true),

  dpp_sub.dependency('dpp'),
  libassert_sub.dependency('assert'),

  dependency('spdlog', required: true),
  dependency(
    'gtkmm-4.0',
    required: true,
    allow_fallback: true
  ),
]

haumea_compilation_options = [
  'cpp_std=c++20'
]

include_dirs = [
  include_directories('include')
]

sources = files(
  'src/env.cpp'
)

haumea_dep = declare_dependency(
  sources: sources,
  dependencies: local_deps,
  include_directories: include_dirs,
)

subdir('src')
subdir('test')
