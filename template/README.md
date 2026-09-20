# @QB_NAME@

A [qb](https://github.com/isndev/qb) actor-framework project, scaffolded from
[`qb-sample-project`](https://github.com/isndev/qb-sample-project) against qb **@QB_VERSION@**.

It is a working HTTP service, a test suite and a CI workflow — not a stub. Replace the parts you
do not want; the structure is the point.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/bin/@QB_NAME@
```

Then, in another terminal:

```bash
curl localhost:8080/health
curl 'localhost:8080/?name=ada'
```

There are **no submodules to initialise**. qb and the qbm modules are fetched by CMake at
configure time, at the ref recorded in `CMakeLists.txt` as `QB_GIT_REF` — written there by the
scaffolder from the version of qb it shipped with. Nothing in this repository stores a
dependency commit that can quietly go stale.

Requirements: a C++20 compiler, CMake ≥ 3.24, and OpenSSL (qb builds without it, but qbm-http's
HTTP/2, WebSocket and JWT features do not).

## Test

```bash
cmake -S . -B build -D@QB_NAME_UPPER@_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

```
    Start 1: @QB_NAME@-test-unit-greeting
    Start 2: @QB_NAME@-test-system-actor-messaging
```

The names are the ecosystem's convention — `<project>-test-<tier>-<name>`, with `module:` and
`tier:` labels, so `ctest -L tier:unit` selects the fast ones. `tests/CMakeLists.txt` explains
what each tier is allowed to touch and why this project registers its tests itself rather than
through `qb_add_test()`.

## Layout

```
CMakeLists.txt          dependency resolution, targets, options — start here
src/
  app/greeting.h|.cpp   plain logic: no actors, no I/O. Unit-testable by calling it.
  app/api_actor.h       the HTTP server actor. The three things to copy are in its header.
  main.cpp              argument parsing and engine startup, nothing else
tests/
  unit/                 pure logic, no engine
  system/               a real qb::Main, real actors, nothing outside the process
qbm/                    drop your own qbm modules here; they load automatically
.github/workflows/ci.yml
```

Logic lives in `src/app/`, not in `main.cpp`, so the tests link exactly what ships. A rule that
only exists inside `main()` can only be tested by running the program.

## The two things newcomers get wrong

**`onInit()` is a coroutine.** It returns `qb::io::async::task<bool>` and ends in `co_return`.
The pre-2.6 `bool onInit()` does not compile. `co_return false` fails the actor cleanly.

**Actors do not share memory.** Each actor is pinned to a `VirtualCore` — one thread — and
processes one message at a time, so its own state needs no locking. State reached from two
actors needs no locking either, because you must not have any: pass events instead. `push()` is
ordered and takes any event; it is `noexcept`, so a throwing event constructor calls
`std::terminate()`.

Time is `qb::duration` / `qb::mono_time` / `qb::wall_time`. `qb::Timestamp` and
`<qb/system/timestamp.h>` were removed in 3.0.

## Adding a qbm module

The official ones are fetched — trim or extend `@QB_NAME_UPPER@_MODULES` in `CMakeLists.txt`:

```cmake
set(@QB_NAME_UPPER@_MODULES http pgsql redis CACHE STRING "...")
```

Your own go in `qbm/`, where they are picked up automatically and exposed as `qbm::<name>`:

```bash
cd qbm
curl -fsSL https://raw.githubusercontent.com/isndev/qb/main/script/qb-new-module.sh | bash /dev/stdin mymodule
```

## Working on qb itself

Point CMake at your checkout — no edit to `CMakeLists.txt`:

```bash
cmake -S . -B build -DFETCHCONTENT_SOURCE_DIR_QB=/path/to/qb
```

The same works per module: `FETCHCONTENT_SOURCE_DIR_QBM-HTTP`, `..._QBM-PGSQL`, `..._QBM-REDIS`.

## Pinning for production

`QB_GIT_REF` is whatever the scaffolder resolved on the day you ran it. If it names a **branch**,
your build follows a moving line — fine while you are developing, not fine for a release. Set it
to a released tag once you want reproducible builds:

```cmake
set(QB_GIT_REF "v3.2.0" CACHE STRING "...")
```

## Documentation

- [qb](https://github.com/isndev/qb) — framework, guides and API reference
- [qbm-http](https://github.com/isndev/qbm-http) · [qbm-pgsql](https://github.com/isndev/qbm-pgsql) · [qbm-redis](https://github.com/isndev/qbm-redis)
- Both repositories publish `llms.txt` for coding assistants.

## License

The template carries Apache-2.0, matching qb. This project is yours — replace this section and
the `LICENSE` file with whatever you actually ship under.
