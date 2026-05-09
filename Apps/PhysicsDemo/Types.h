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

struct BodyState
{
    int id = 0;
    Vec3 position = {};
    Quat rotation = {};
    Vec3 halfExtents = {};
    float radius = 0;
    bool isSphere = true;

    MIRO_REFLECT(id, position, rotation, halfExtents, radius, isSphere)
};

struct WorldState
{
    double time = 0;
    std::vector<BodyState> bodies = {};

    MIRO_REFLECT(time, bodies)
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
