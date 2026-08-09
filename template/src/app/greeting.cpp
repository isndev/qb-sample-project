#include "app/greeting.h"

#include <string>
#include <string_view>
#include <qb/json.h>

namespace @QB_NAME_LOWER@ {

std::string_view
version() noexcept {
    return @QB_NAME_UPPER@_VERSION;
}

qb::json
greeting(std::string_view who) {
    if (who.empty()) {
        who = "world";
    }
    return qb::json{
        {"message", "hello, " + std::string{who}},
        {"service", "@QB_NAME@"},
        {"version", std::string{version()}}
    };
}

} // namespace @QB_NAME_LOWER@
