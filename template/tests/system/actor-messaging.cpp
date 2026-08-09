/**
 * @file tests/system/actor-messaging.cpp
 * @brief System tier — a real qb::Main, real actors, real events, nothing outside this process.
 *
 * This is the shape almost every qb test ends up having, so it is worth reading once:
 *
 *  - The fixtures live in a NAMED namespace, never an anonymous one. A type with no linkage that
 *    reaches a framework template's coroutine frame is a -Wsubobject-linkage error on gcc, and
 *    the compiler diagnoses only some of the cases — so the convention is applied everywhere
 *    rather than where a compiler happened to complain.
 *  - Every in-actor expectation is ALSO mirrored to a process-global atom that the test body
 *    asserts after join(). An EXPECT_* that runs on a worker thread and never executes cannot
 *    fail; without the mirror, an actor that silently never ran would look like a pass.
 *  - Nothing sleeps and nothing polls a clock. The actors terminate themselves and the engine
 *    drains; ctest's TIMEOUT is the only backstop. A test that waits a fixed number of
 *    milliseconds is a test that is flaky on a loaded machine.
 */

#include <atomic>
#include <cstdint>
#include <gtest/gtest.h>
#include <qb/actor.h>
#include <qb/main.h>

namespace actor_messaging_test {

std::atomic<int>  g_pings_received{0};
std::atomic<int>  g_pongs_received{0};
std::atomic<bool> g_reply_came_from_responder{false};

constexpr int kRounds = 8;

struct Ping : qb::Event {};
struct Pong : qb::Event {};

/// Replies to whoever asked. `getSource()` is the sender's ActorId, which is how a reply finds
/// its way back without either actor holding a pointer to the other.
class Responder : public qb::Actor {
public:
    qb::io::async::task<bool>
    onInit() override {
        registerEvent<Ping>(*this);
        co_return true;
    }

    void
    on(const Ping &event) {
        g_pings_received.fetch_add(1);
        push<Pong>(event.getSource());
    }
};

/// Sends kRounds pings, counts the replies, then stops the engine. Owning the shutdown here is
/// what keeps the test deterministic: the engine ends because the work ended.
class Requester : public qb::Actor {
public:
    explicit Requester(qb::ActorId responder) noexcept
        : _responder(responder) {}

    qb::io::async::task<bool>
    onInit() override {
        registerEvent<Pong>(*this);
        for (int i = 0; i < kRounds; ++i) {
            push<Ping>(_responder);
        }
        co_return true;
    }

    void
    on(const Pong &event) {
        // ActorId has no operator==; it converts to uint32_t, so compare that explicitly
        // rather than leaning on the implicit conversion.
        if (static_cast<std::uint32_t>(event.getSource()) ==
            static_cast<std::uint32_t>(_responder)) {
            g_reply_came_from_responder.store(true);
        }
        if (g_pongs_received.fetch_add(1) + 1 == kRounds) {
            kill();
            qb::Main::stop();
        }
    }

private:
    const qb::ActorId _responder;
};

} // namespace actor_messaging_test

TEST(ActorMessaging, EveryPingIsAnsweredExactlyOnce) {
    using namespace actor_messaging_test;

    g_pings_received.store(0);
    g_pongs_received.store(0);
    g_reply_came_from_responder.store(false);

    qb::Main engine;

    // Both actors on core 0: a VirtualCore runs one actor at a time, so this is a single thread
    // and the exchange is ordered. Putting the responder on core 1 also works and is how you
    // would actually scale — the events then cross a lock-free queue instead of a local one.
    const auto responder = engine.addActor<Responder>(0);
    ASSERT_TRUE(responder.is_valid());
    engine.addActor<Requester>(0, responder);

    engine.start();
    engine.join();

    EXPECT_FALSE(engine.hasError());
    EXPECT_EQ(kRounds, g_pings_received.load());
    EXPECT_EQ(kRounds, g_pongs_received.load());
    EXPECT_TRUE(g_reply_came_from_responder.load());
}
