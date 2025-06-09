# qb-sample-project

Minimal boilerplate to kickstart a C++17 project using the **qb** framework (actor model-based).

---

## ⚙️ Requirements

- C++17 compiler (GCC ≥ 7, Clang ≥ 4, MSVC ≥ 19.11, or Xcode ≥ 10.2)
- CMake ≥ 3.13
- `qb/` directory with the qb framework cloned
- `qbm/` directory with qb modules (e.g., `qbm-http`)
- Optional: enable tests with `-DQB_BUILD_TEST=ON`

---

## 🧱 Project Structure

```
/
├── qb/              ← qb framework
├── qbm/             ← qb modules (e.g., qbm-http)
├── examples/        ← usage examples
├── src/main.cpp     ← main application entry point
├── test/            ← unit tests (optional)
└── CMakeLists.txt   ← build configuration
```

---

## 🚀 Build Instructions

From the project root:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DQB_BUILD_TEST=ON ..
make
```

- The resulting binary will be: `build/qb-sample-project`
- To skip tests: omit the `-DQB_BUILD_TEST` flag

---

## 🧪 Running Tests

If compiled with tests enabled:

```bash
cd build
ctest --output-on-failure
```

---

## 🔗 Dependencies

This project links with:
- **qb-core** (core runtime)
- **qbm-http** (HTTP module)

Handled via CMake:

```cmake
target_link_libraries(qb-sample-project qb-core qbm-http)
```

---

## 📚 Documentation & Resources

- **qb**: C++ Actor Model framework for scalable concurrency
- See `examples/` for reference use cases
- For advanced CMake setups: [awesome-cmake](https://github.com/onqtam/awesome-cmake)

---

## 🛠️ Customization

- Add your sources to `src/`, custom modules to `qbm/`
- Update `target_link_libraries()` in CMake as needed
- Enable extra build options (e.g. `-DQB_BUILD_BENCHMARK=ON`) if supported

---

## 📝 License

Apache‑2.0 (see `qb/LICENSE`)

---

## ✅ Contribution

1. Fork this repo
2. Create a feature branch: `feature/your-change`
3. Add code, tests, docs
4. Open a Pull Request

---

## 🔚 Summary

This README provides a quick reference for building and extending a project using **qb**. If you need help integrating new modules, CI/CD, or packaging—ask directly.