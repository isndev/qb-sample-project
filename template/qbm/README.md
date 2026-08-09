# Local qbm modules

Anything you drop in this directory is compiled from source and exposed as `qbm::<name>`,
exactly like the official modules the root `CMakeLists.txt` fetches. That happens through the
`qb_load_modules()` call in the root `CMakeLists.txt`, which globs this directory.

Scaffold one here:

```bash
cd qbm
curl -fsSL https://raw.githubusercontent.com/isndev/qb/main/script/qb-new-module.sh | bash /dev/stdin mymodule
```

Then link it:

```cmake
target_link_libraries(@QB_NAME@-lib PUBLIC qbm::mymodule)
```

A module scaffolded this way also carries its own superbuild root, so you can build and test it
in isolation without configuring this project — see its `README.md`.

This file is here so the directory exists in git; delete it once you have a real module.
