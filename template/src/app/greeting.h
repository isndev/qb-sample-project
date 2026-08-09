/**
 * @file src/app/greeting.h
 * @brief Plain application logic for @QB_NAME@ — no actors, no I/O, no framework.
 *
 * This file exists to make a point about where logic belongs. Everything here is a pure
 * function over its arguments, which means the unit test in tests/unit/ can call it directly
 * without starting an engine, opening a socket or waiting for anything. Actors are for
 * concurrency and I/O; they are not where your rules live.
 */

#ifndef @QB_NAME_UPPER@_APP_GREETING_H
#define @QB_NAME_UPPER@_APP_GREETING_H

#include <string>
#include <string_view>
#include <qb/json.h>

namespace @QB_NAME_LOWER@ {

/// The version this binary reports, composed at compile time from CMake's project version.
[[nodiscard]] std::string_view version() noexcept;

/**
 * @brief Build the JSON body served by `GET /`.
 * @param who Name to greet; empty falls back to "world".
 *
 * Returns a `qb::json`, which is `nlohmann::json` — qb re-exports it rather than shipping its
 * own copy, so anything you know about nlohmann applies. Note that qb 3.0 requires a real
 * nlohmann package for an INSTALLABLE build; a plain build like this one fetches it.
 */
[[nodiscard]] qb::json greeting(std::string_view who);

} // namespace @QB_NAME_LOWER@

#endif // @QB_NAME_UPPER@_APP_GREETING_H
