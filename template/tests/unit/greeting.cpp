/**
 * @file tests/unit/greeting.cpp
 * @brief Unit tier — pure logic, no engine, no sockets.
 *
 * The whole point of keeping greeting() free of framework types is that this file needs nothing
 * but a function call. If a test of your business rules has to start a qb::Main to run, the
 * rules are in the wrong place.
 */

#include <string>
#include <gtest/gtest.h>
#include <qb/json.h>

#include "app/greeting.h"

TEST(Greeting, NamesTheCaller) {
    const auto body = @QB_NAME_LOWER@::greeting("ada");
    EXPECT_EQ("hello, ada", body.at("message").get<std::string>());
    EXPECT_EQ("@QB_NAME@", body.at("service").get<std::string>());
}

TEST(Greeting, FallsBackToWorldOnEmptyInput) {
    // An absent ?name= query parameter arrives as an empty string, not as an error, so this is
    // the path a plain `GET /` takes.
    const auto body = @QB_NAME_LOWER@::greeting("");
    EXPECT_EQ("hello, world", body.at("message").get<std::string>());
}

TEST(Greeting, ReportsTheVersionCMakeCompiledIn) {
    // Guards the CMake plumbing, not the string: @QB_NAME_UPPER@_VERSION comes from
    // project(VERSION) and a broken definition would show up here as an empty value rather than
    // as a puzzling `--version` output much later.
    EXPECT_FALSE(@QB_NAME_LOWER@::version().empty());
    EXPECT_EQ(@QB_NAME_LOWER@::version(),
              @QB_NAME_LOWER@::greeting("x").at("version").get<std::string>());
}
