local_deps = [
  dependency('catch2-with-main', required: true, static: false),
]

test_sources = files()

test_exe = executable(
  'haumea_test',
  test_sources,
  dependencies: [ haumea_dep, local_deps ],
  override_options: haumea_compilation_options
)

tests = []

foreach test : tests
  test(test, test_exe)
endforeach
