#include "WorldApi.h"

ResetResponse reset()
{
    resetWorld();
    return {.ok = true};
}

LaunchResponse launchBall(const LaunchRequest& request)
{
    return {.id = launchBallInWorld(request)};
}

MIRO_EXPORT_COMMANDS(reset, launchBall)

// Event payload types: not reachable through any command, so register
// them explicitly so the schema codegen emits TS interfaces for them.
MIRO_EXPORT_TYPES(WorldState, BodyState, Quat)
