//
// Created by isnDev on 3/11/2019.
//

#include <qb/main.h>
#include <qb-module/timer/service/Scheduler.h>

int main() {
    qb::Main main({0});
    main.core(0)
            .addActor<qbm::timer::service::Scheduler>();

    main.start();
    main.stop();
    main.join();
    return 0;
}
