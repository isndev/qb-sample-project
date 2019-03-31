
#include <qb/main.h>
#include <qb-module/timer/service/Scheduler.h>

struct SampleTimedEvent : public qbm::timer::event::Timed {
    SampleTimedEvent(qb::Timespan const &span, uint32_t const repeat = 1)
        : Timed(span, repeat)
    {}
};

class ActorSample : public qb::Actor {
public:
    bool onInit() override final {
        registerEvent<SampleTimedEvent>(*this);
        push<SampleTimedEvent>(getServiceId<qbm::timer::service::Tag>(0), qb::Timespan::seconds(3));
        return true;
    }

    void on(SampleTimedEvent const &event) {
        broadcast<qb::KillEvent>();
    }
};

int main() {
    qb::Main main({0});
    main.core(0)
            .addActor<qbm::timer::service::Scheduler>()
            .addActor<ActorSample>();

    main.start();
    main.join();
    return 0;
}
