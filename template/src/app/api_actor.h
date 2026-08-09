/**
 * @file src/app/api_actor.h
 * @brief The HTTP server actor for @QB_NAME@.
 *
 * A qb actor that is also an HTTP server. The two base classes are not a trick: qb::Actor gives
 * it a mailbox and a place on a core, qb::http::Server<> gives it a listener and a router, and
 * because a VirtualCore runs one actor at a time, every handler below is already serialised
 * against every other. There are no locks in this file and there should be none in yours.
 *
 * THE THREE THINGS TO COPY FROM THIS FILE
 *
 * 1. `onInit()` returns `qb::io::async::task<bool>` and ends in `co_return`. It is a coroutine
 *    since 2.6 — the pre-2.6 `bool onInit()` no longer compiles. `co_return false` fails the
 *    actor's construction cleanly; throwing is a different, louder outcome.
 * 2. Routes are registered, then `router().compile()` is called ONCE. Compiling builds the match
 *    structure; adding routes after it has no effect on matching.
 * 3. `listen()` is checked. A port already in use returns false, and an actor that ignores it
 *    starts an engine that serves nothing.
 */

#ifndef @QB_NAME_UPPER@_APP_API_ACTOR_H
#define @QB_NAME_UPPER@_APP_API_ACTOR_H

#include <cstdint>
#include <string>
#include <qb/actor.h>
#include <qb/io.h>
#include <qbm/http/http.h>

#include "app/greeting.h"

namespace @QB_NAME_LOWER@ {

class ApiActor
    : public qb::Actor
    , public qb::http::Server<> {
public:
    explicit ApiActor(std::uint16_t port) noexcept
        : _port(port) {}

    qb::io::async::task<bool>
    onInit() override {
        router().get("/", [](auto ctx) {
            ctx->response().status() = qb::http::Status::OK;
            ctx->response().add_header("Content-Type", "application/json");
            ctx->response().body() = greeting(ctx->request().query("name"));
            ctx->complete(qb::http::AsyncTaskResult::COMPLETE);
        });

        // A liveness endpoint that says what it is running. CI curls exactly this.
        router().get("/health", [](auto ctx) {
            ctx->response().status() = qb::http::Status::OK;
            ctx->response().add_header("Content-Type", "application/json");
            ctx->response().body() = qb::json{
                {"status", "ok"},
                {"service", "@QB_NAME@"},
                {"version", std::string{version()}}
            };
            ctx->complete(qb::http::AsyncTaskResult::COMPLETE);
        });

        router().compile();

        if (!listen(qb::io::uri("tcp://0.0.0.0:" + std::to_string(_port)))) {
            qb::io::cout() << "@QB_NAME@: cannot listen on port " << _port << '\n';
            co_return false;
        }
        start();

        qb::io::cout() << "@QB_NAME@ " << version() << " listening on http://localhost:"
                       << _port << " (GET / , GET /health)\n";
        co_return true;
    }

    // Sent by the engine on shutdown. Without a handler the default terminates the actor anyway;
    // this one exists so there is an obvious place to flush or close things you own.
    void
    on(const qb::KillEvent &) noexcept {
        qb::io::cout() << "@QB_NAME@: shutting down\n";
        kill();
    }

private:
    const std::uint16_t _port;
};

} // namespace @QB_NAME_LOWER@

#endif // @QB_NAME_UPPER@_APP_API_ACTOR_H
