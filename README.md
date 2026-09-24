# qb-sample-project

The project template behind `qb-new-project.sh`. It is **not** a project you clone and use — it
is the payload a scaffolder renders. Start a project like this:

```bash
curl -fsSL https://raw.githubusercontent.com/isndev/qb/main/script/qb-new-project.sh | bash /dev/stdin MyProject
cd MyProject
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/bin/MyProject
```

That produces a fresh git repository with one commit, named after you throughout, depending on a
qb the scaffolder chose — no submodules, no history that is not yours.

## Layout

```
template/     the payload. Everything here becomes the user's project.
README.md     this file — about the template, never copied
LICENSE
.github/workflows/template.yml
```

`template/` is a directory rather than "the repository minus an exclusion list" on purpose: an
exclusion list is a thing you forget to update, and the failure is silent — the user's project
quietly inherits the template's own CI or README.

## Placeholders

The scaffolder owns the substitution vocabulary; this template may only use these tokens, in file
contents **and** in path names. An unknown `@QB_...@` token is a hard error at scaffold time, so
adding one here without teaching the scaffolder about it fails loudly rather than shipping a
half-rendered tree.

| Token               | Becomes                                        | Example (`MyProject`) |
|---------------------|------------------------------------------------|-----------------------|
| `@QB_NAME@`         | the name the user passed, verbatim              | `MyProject`           |
| `@QB_NAME_LOWER@`   | lowercased — C++ namespaces                     | `myproject`           |
| `@QB_NAME_UPPER@`   | uppercased — include guards, CMake options      | `MYPROJECT`           |
| `@QB_REF@`          | the isndev/qb git ref the project builds against| `develop`             |
| `@QB_VERSION@`      | the qb version the scaffolder shipped with      | `3.2.1`               |
| `@QB_TEMPLATE_REF@` | the ref of this template that was used          | `develop`             |

## How this template stays current

It did not, for seven years, and the reason was structural rather than neglect: nothing bound a
template version to a qb version, and nothing ever built the template. Both are fixed, and both
fixes matter.

**Version binding.** The old template stored qb as a submodule gitlink — a pinned dependency
living in the template, which can only drift away from the qb whose script cloned it. It drifted
to a commit from before v2.0.0 while continuing to build against that pin. Nothing is stored now:
`qb-new-project.sh` resolves the ref from `QB_FRAMEWORK_VERSION` in the qb it ships with, and
writes it into the generated `CMakeLists.txt`. The one-liner's URL therefore selects the pairing —
`.../qb/main/script/...` is the released line, `.../qb/v3.2.1/script/...` is that release. The
scaffolder reports which ref it used and why, including when it had to fall back.

Branches here follow qb's: **`develop`** is the next version, **`main`**/**`master`** the released
line. The scaffolder prefers a `v<version>` tag, falls back to the development line while that
version is unreleased, and says so either way.

**A lane that can fail.** `.github/workflows/template.yml` runs qb's actual `qb-new-project.sh`
against this checkout, then configures, builds, tests and *curls* the result on Linux and macOS,
weekly and on every push. It asserts a non-zero translation-unit count, that no placeholder
survived, that the generated repository has exactly one commit and no remote, that the suite
executed rather than skipped, and that the server returns the expected JSON body. Drift is now a
red build instead of a discovery.

## Changing the template

Render it locally with the real scaffolder — no push required:

```bash
git clone https://github.com/isndev/qb
cd /tmp && QB_TEMPLATE_DIR=/path/to/qb-sample-project \
  bash /path/to/qb/script/qb-new-project.sh Scratch
cd Scratch && cmake -S . -B build -DSCRATCH_BUILD_TESTS=ON && cmake --build build --parallel
```

`QB_TEMPLATE_DIR` is what the CI lane uses too, which is the point: the thing you run locally is
the thing that gates the pull request.

Keep `template/` buildable *after rendering*, not before — it is not expected to configure with
the placeholders in place.

## License

Apache-2.0, matching qb. See `LICENSE`.
