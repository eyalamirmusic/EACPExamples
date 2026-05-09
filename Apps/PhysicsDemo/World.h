#pragma once

#include "Types.h"
#include <reactphysics3d/reactphysics3d.h>
#include <vector>

class World
{
public:
    World();

    void step(float timeStep);
    void reset();
    SceneSnapshot sceneSnapshot() const;
    WorldTick tickSnapshot();
    int launchBall(const LaunchRequest& request);
    void setRainEnabled(bool enabled);

    World(const World&) = delete;
    World& operator=(const World&) = delete;

private:

    void buildScene();
    void clearBodies();
    void spawnRainSphere();
    void spawnFountainSphere();
    void stir();
    void explosion();

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
    bool rainEnabled = false;
    int tickCounter = 0;
    std::vector<int> pendingRemovedIds = {};
};
