# @QB_NAME@ test suite.
#
# NAMING AND LABELS ARE THE ECOSYSTEM'S, THE REGISTRATION IS OURS
# ---------------------------------------------------------------
# qb, qbm-http, qbm-pgsql and qbm-redis all register tests through qb_add_test(), which produces
# a target and ctest entry called `<module>-test-<tier>-<name>` plus `module:` and `tier:` labels.
# This project reproduces that exactly — `@QB_NAME@-test-unit-greeting` and friends — but wires it
# by hand, because qb_add_test() only does anything when QB_BUILD_TESTS is ON, and turning that on
# would drag qb's and every fetched module's suite into this build (measured: 176 ctest entries and
# 534 build targets, of which 2 and a handful are yours). The root CMakeLists.txt explains it.
#
# THE TIERS are a contract about what a test may touch:
#   unit/        pure logic. No engine, no sockets, no sleeping. Milliseconds.
#   system/      a real qb::Main with real actors, nothing outside this process.
#   integration/ needs something external (a database, a broker). None here yet; when you add one,
#                label it `live` and make it skip rather than fail where the service is absent —
#                and then never quote a bare `100% tests passed`, because ctest counts a SKIP as a
#                pass. Report executed and skipped separately.
#
# Run them:
#   ctest --test-dir build --output-on-failure           # everything (only this project's)
#   ctest --test-dir build -L tier:unit                  # only the fast ones
#   ctest --test-dir build -R greeting                   # by name

# One helper so a new test is one line and cannot get the convention subtly wrong.
function(@QB_NAME_LOWER@_add_test TIER NAME SOURCE)
    set(target "@QB_NAME@-test-${TIER}-${NAME}")
    add_executable(${target} "${SOURCE}")
    target_link_libraries(${target} PRIVATE @QB_NAME@-lib GTest::gtest_main)
    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
    add_test(NAME ${target} COMMAND ${target})
    set_tests_properties(${target} PROPERTIES
            LABELS "module:@QB_NAME@;tier:${TIER}"
            TIMEOUT 60)
endfunction()

@QB_NAME_LOWER@_add_test(unit   greeting        unit/greeting.cpp)
@QB_NAME_LOWER@_add_test(system actor-messaging system/actor-messaging.cpp)
