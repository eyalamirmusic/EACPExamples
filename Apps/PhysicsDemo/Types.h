#pragma once

#include <Miro/Miro.h>
#include <vector>

struct Vec3
{
    float x = 0;
    float y = 0;
    float z = 0;

    MIRO_REFLECT(x, y, z)
};

struct Quat
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    MIRO_REFLECT(x, y, z, w)
};

struct BodyDescriptor
{
    int id = 0;
    bool isSphere = true;
    float radius = 0;
    Vec3 halfExtents = {};

    MIRO_REFLECT(id, isSphere, radius, halfExtents)
};

struct BodyTransform
{
    int id = 0;
    Vec3 position = {};
    Quat rotation = {};

    MIRO_REFLECT(id, position, rotation)
};

struct SceneSnapshot
{
    std::vector<BodyDescriptor> bodies = {};

    MIRO_REFLECT(bodies)
};

struct WorldTick
{
    double time = 0;
    std::vector<BodyTransform> bodies = {};
    std::vector<int> removedIds = {};

    MIRO_REFLECT(time, bodies, removedIds)
};

struct ResetResponse
{
    bool ok = true;

    MIRO_REFLECT(ok)
};

struct LaunchRequest
{
    Vec3 origin = {};
    Vec3 direction = {};
    float speed = 25;

    MIRO_REFLECT(origin, direction, speed)
};

struct LaunchResponse
{
    int id = 0;

    MIRO_REFLECT(id)
};

struct RainRequest
{
    bool enabled = false;

    MIRO_REFLECT(enabled)
};

struct RainResponse
{
    bool ok = true;

    MIRO_REFLECT(ok)
};
