#include "Types.h"
#include <chrono>

namespace
{
long long currentEpochMillis()
{
    using namespace std::chrono;
    auto now = system_clock::now().time_since_epoch();
    return duration_cast<milliseconds>(now).count();
}
} // namespace

GreetResponse greet(const GreetRequest& request)
{
    auto name = request.name.empty() ? std::string {"world"} : request.name;
    return {.message = "Hello, " + name + "!",
            .serverTimeMs = currentEpochMillis()};
}

MIRO_EXPORT_COMMAND(greet)
