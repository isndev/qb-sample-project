/**
 * @file src/main.cpp
 * @brief Entry point for @QB_NAME@ — argument parsing and engine startup, nothing else.
 *
 * Application logic lives in src/app/ so the test suite can link it directly. Keeping main.cpp
 * thin is not tidiness: a rule that only exists inside main() can only be tested by running the
 * program.
 */

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <qb/main.h>

#include "app/api_actor.h"
#include "app/greeting.h"

namespace {

constexpr std::uint16_t kDefaultPort = 8080;

void
print_usage() {
    std::cout << "usage: @QB_NAME@ [--port <n>] [--version] [--help]\n"
              << "  --port <n>   TCP port to listen on (default " << kDefaultPort << ")\n"
              << "  --version    print the version and exit\n";
}

} // namespace

int
main(int argc, char **argv) {
    std::uint16_t port = kDefaultPort;

    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};
        if (arg == "--version" || arg == "-v") {
            std::cout << "@QB_NAME@ " << @QB_NAME_LOWER@::version() << '\n';
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            print_usage();
            return 0;
        }
        if (arg == "--port" && i + 1 < argc) {
            port = static_cast<std::uint16_t>(std::strtoul(argv[++i], nullptr, 10));
            continue;
        }
        std::cerr << "@QB_NAME@: unrecognised argument '" << arg << "'\n";
        print_usage();
        return 2;
    }

    try {
        qb::Main engine;

        // Core 0. Actors are pinned to a VirtualCore, and each core is one thread: to scale, add
        // actors on more cores rather than threads inside one.
        engine.addActor<@QB_NAME_LOWER@::ApiActor>(0, port);

        engine.start();
        engine.join();

        // The engine reports a failed actor init through hasError(), not through an exception —
        // a listen() that returned false ends up here, and exiting 0 on it would be a lie.
        if (engine.hasError()) {
            std::cerr << "@QB_NAME@: the engine stopped with an error\n";
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << "@QB_NAME@: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
