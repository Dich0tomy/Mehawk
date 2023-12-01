catch2 = dependency('catch2-with-main', required: true)

local_deps = [
  catch2
]

test_sources = files(
  'toml_config.cpp'
)

test_exe = executable(
  'mehawk_test',
  test_sources,
  include_directories: include_directories('.'),
  dependencies: [ mehawk_dep, local_deps ],
  override_options: mehawk_compilation_options,
)

tests = [
  'Empty config returns defaults',
  'Config is read properly',
  'Config is written properly'
]

foreach test : tests
  test(test, test_exe)
endforeach
