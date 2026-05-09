#pragma once

#include "Types.h"
#include <reactphysics3d/reactphysics3d.h>
#include <vector>

class World
{
public:
    static World& instance();

    void step(float timeStep);
    void reset();
    WorldState snapshot() const;
    int launchBall(const LaunchRequest& request);

    World(const World&) = delete;
    World& operator=(const World&) = delete;

private:
    World();

    void buildScene();
    void clearBodies();

    struct Body
    {
        int id = 0;
        reactphysics3d::RigidBody* body = nullptr;
        bool isSphere = true;
        float radius = 0;
        Vec3 halfExtents = {};
    };

    reactphysics3d::PhysicsCommon common = {};
    reactphysics3d::PhysicsWorld* world = nullptr;
    std::vector<Body> bodies = {};
    double simulatedTime = 0;
    int nextId = 0;
};
