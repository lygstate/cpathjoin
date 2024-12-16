cpjtest_sources = files(
    'main.c',
    'absolute_test.c',
    'basename_test.c',
    'dirname_test.c',
    'extension_test.c',
    'guess_test.c',
    'intersection_test.c',
    'is_absolute_test.c',
    'is_relative_test.c',
    'join_test.c',
    'normalize_test.c',
    'relative_test.c',
    'root_test.c',
    'windows_test.c',
)

cpjtest = executable('cpjtest',
    sources: cpjtest_sources,
    dependencies: cpj_dep,
)
test('cpjtest', cpjtest)
