#include "World.h"
#include "WorldApi.h"

namespace rp = reactphysics3d;

namespace
{
Vec3 toVec3(const rp::Vector3& v)
{
    return {(float) v.x, (float) v.y, (float) v.z};
}

Quat toQuat(const rp::Quaternion& q)
{
    return {(float) q.x, (float) q.y, (float) q.z, (float) q.w};
}
} // namespace

World& World::instance()
{
    static World w;
    return w;
}

World::World()
{
    world = common.createPhysicsWorld();
    buildScene();
}

void World::step(float timeStep)
{
    world->update(timeStep);
    simulatedTime += timeStep;
}

void World::reset()
{
    clearBodies();
    simulatedTime = 0;
    nextId = 0;
    buildScene();
}

WorldState World::snapshot() const
{
    auto state = WorldState {};
    state.time = simulatedTime;
    state.bodies.reserve(bodies.size());

    for (const auto& b: bodies)
    {
        const auto& transform = b.body->getTransform();
        auto entry = BodyState {};
        entry.id = b.id;
        entry.position = toVec3(transform.getPosition());
        entry.rotation = toQuat(transform.getOrientation());
        entry.isSphere = b.isSphere;
        entry.radius = b.radius;
        entry.halfExtents = b.halfExtents;
        state.bodies.push_back(entry);
    }

    return state;
}

void World::clearBodies()
{
    for (const auto& b: bodies)
        world->destroyRigidBody(b.body);
    bodies.clear();
}

void resetWorld() { World::instance().reset(); }
WorldState snapshotWorld() { return World::instance().snapshot(); }
void stepWorld(float timeStep) { World::instance().step(timeStep); }
int launchBallInWorld(const LaunchRequest& request)
{
    return World::instance().launchBall(request);
}

int World::launchBall(const LaunchRequest& request)
{
    auto radius = 0.35f;
    auto origin = rp::Vector3 {
        request.origin.x, request.origin.y, request.origin.z};
    auto transform = rp::Transform {origin, rp::Quaternion::identity()};
    auto* body = world->createRigidBody(transform);
    body->setType(rp::BodyType::DYNAMIC);
    body->addCollider(common.createSphereShape(radius), rp::Transform::identity());
    body->setLinearVelocity({request.direction.x * request.speed,
                             request.direction.y * request.speed,
                             request.direction.z * request.speed});

    auto id = nextId++;
    bodies.push_back({id, body, true, radius, {}});
    return id;
}

void World::buildScene()
{
    auto identity = rp::Transform::identity();

    auto groundExtents = rp::Vector3 {25, 0.5f, 25};
    auto groundShape = common.createBoxShape(groundExtents);
    auto groundTransform = rp::Transform {{0, -0.5f, 0}, rp::Quaternion::identity()};
    auto* ground = world->createRigidBody(groundTransform);
    ground->setType(rp::BodyType::STATIC);
    ground->addCollider(groundShape, identity);
    bodies.push_back({nextId++,
                      ground,
                      false,
                      0,
                      {groundExtents.x, groundExtents.y, groundExtents.z}});

    constexpr auto cols = 5;
    constexpr auto rows = 8;
    constexpr auto layers = 2;
    constexpr auto half = 0.5f;
    constexpr auto spacing = half * 2.02f;
    constexpr auto wallZ = -4.0f;

    for (auto layer = 0; layer < layers; ++layer)
        for (auto row = 0; row < rows; ++row)
            for (auto col = 0; col < cols; ++col)
            {
                auto x = ((float) col - (cols - 1) * 0.5f) * spacing;
                auto y = half + (float) row * spacing;
                auto z = wallZ - (float) layer * spacing;

                auto extents = rp::Vector3 {half, half, half};
                auto shape = common.createBoxShape(extents);
                auto transform = rp::Transform {{x, y, z}, rp::Quaternion::identity()};
                auto* body = world->createRigidBody(transform);
                body->setType(rp::BodyType::DYNAMIC);
                body->addCollider(shape, identity);
                bodies.push_back(
                    {nextId++, body, false, 0, {half, half, half}});
            }

    for (auto i = 0; i < 10; ++i)
    {
        auto radius = 0.4f;
        auto x = ((float) i - 4.5f) * 0.9f;
        auto y = 5.0f + (float) (i % 4) * 1.2f;
        auto z = 2.0f + (float) (i % 3) * 0.6f;

        auto shape = common.createSphereShape(radius);
        auto transform = rp::Transform {{x, y, z}, rp::Quaternion::identity()};
        auto* body = world->createRigidBody(transform);
        body->setType(rp::BodyType::DYNAMIC);
        body->addCollider(shape, identity);
        bodies.push_back({nextId++, body, true, radius, {}});
    }
}
