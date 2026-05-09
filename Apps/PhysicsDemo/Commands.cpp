#include "WorldApi.h"

ResetResponse reset()
{
    resetWorld();
    return {.ok = true};
}

SceneSnapshot getScene()
{
    return getSceneFromWorld();
}

LaunchResponse launchBall(const LaunchRequest& request)
{
    return {.id = launchBallInWorld(request)};
}

RainResponse setRain(const RainRequest& request)
{
    setRainInWorld(request.enabled);
    return {.ok = true};
}

MIRO_EXPORT_COMMANDS(reset, getScene, launchBall, setRain)

MIRO_EXPORT_TYPES(SceneSnapshot, WorldTick, BodyDescriptor, BodyTransform, Quat)
