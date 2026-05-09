#include "World.h"
#include "WorldApi.h"

#include <cmath>
#include <cstdlib>

namespace rp = reactphysics3d;

namespace
{
constexpr int kBodyCap = 5000;
constexpr int kStirEvery = 30;       // 4 Hz at 120Hz
constexpr int kExplodeEvery = 240;   // 0.5 Hz at 120Hz
constexpr int kFountainEvery = 18;   // ~6.7 Hz at 120Hz
constexpr int kRainEvery = 6;        // 20 Hz at 120Hz when toggled on

Vec3 toVec3(const rp::Vector3& v)
{
    return {(float) v.x, (float) v.y, (float) v.z};
}

Quat toQuat(const rp::Quaternion& q)
{
    return {(float) q.x, (float) q.y, (float) q.z, (float) q.w};
}

float randomFloat(float lo, float hi)
{
    auto u = (float) std::rand() / (float) RAND_MAX;
    return lo + u * (hi - lo);
}
} // namespace

World::World()
{
    world = common.createPhysicsWorld();
    buildScene();
}

void World::step(float timeStep)
{
    ++tickCounter;

    auto roomForMore = (int) bodies.size() < kBodyCap;

    if (tickCounter % kStirEvery == 0)
        stir();

    if (tickCounter % kExplodeEvery == 0)
        explosion();

    if (tickCounter % kFountainEvery == 0 && roomForMore)
        spawnFountainSphere();

    if (rainEnabled && tickCounter % kRainEvery == 0 && roomForMore)
        spawnRainSphere();

    world->update(timeStep);
    simulatedTime += timeStep;
}

void World::reset()
{
    clearBodies();
    pendingRemovedIds.clear();
    simulatedTime = 0;
    nextId = 0;
    tickCounter = 0;
    buildScene();
}

void World::setRainEnabled(bool enabled)
{
    rainEnabled = enabled;
}

SceneSnapshot World::sceneSnapshot() const
{
    auto out = SceneSnapshot {};
    out.bodies.reserve(bodies.size());

    for (const auto& b: bodies)
    {
        auto entry = BodyDescriptor {};
        entry.id = b.id;
        entry.isSphere = b.isSphere;
        entry.radius = b.radius;
        entry.halfExtents = b.halfExtents;
        out.bodies.push_back(entry);
    }

    return out;
}

WorldTick World::tickSnapshot()
{
    auto out = WorldTick {};
    out.time = simulatedTime;
    out.bodies.reserve(64);

    for (const auto& b: bodies)
    {
        if (b.body->isSleeping())
            continue;

        const auto& transform = b.body->getTransform();
        auto entry = BodyTransform {};
        entry.id = b.id;
        entry.position = toVec3(transform.getPosition());
        entry.rotation = toQuat(transform.getOrientation());
        out.bodies.push_back(entry);
    }

    out.removedIds = std::move(pendingRemovedIds);
    pendingRemovedIds.clear();
    return out;
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

void World::clearBodies()
{
    for (const auto& b: bodies)
        world->destroyRigidBody(b.body);
    bodies.clear();
}

void World::spawnRainSphere()
{
    auto radius = 0.3f;
    auto x = randomFloat(-14, 14);
    auto z = randomFloat(-14, 14);
    auto transform = rp::Transform {{x, 18.0f, z}, rp::Quaternion::identity()};
    auto* body = world->createRigidBody(transform);
    body->setType(rp::BodyType::DYNAMIC);
    body->addCollider(common.createSphereShape(radius), rp::Transform::identity());
    bodies.push_back({nextId++, body, true, radius, {}});
}

void World::spawnFountainSphere()
{
    constexpr auto twoPi = 6.2831853f;
    auto angle = randomFloat(0, twoPi);
    auto outward = randomFloat(2, 6);
    auto upward = randomFloat(9, 16);
    auto radius = 0.3f;

    auto transform = rp::Transform {{0, 1.5f, 0}, rp::Quaternion::identity()};
    auto* body = world->createRigidBody(transform);
    body->setType(rp::BodyType::DYNAMIC);
    body->addCollider(common.createSphereShape(radius), rp::Transform::identity());
    body->setLinearVelocity({std::cos(angle) * outward,
                             upward,
                             std::sin(angle) * outward});
    bodies.push_back({nextId++, body, true, radius, {}});
}

void World::stir()
{
    if (bodies.size() < 2)
        return;

    for (auto i = 0; i < 6; ++i)
    {
        auto idx = (std::size_t) std::rand() % bodies.size();
        auto& b = bodies[idx];
        if (b.id == 0)
            continue;
        if (b.body->getType() == rp::BodyType::STATIC)
            continue;

        auto current = b.body->getLinearVelocity();
        b.body->setLinearVelocity({current.x + randomFloat(-3, 3),
                                   current.y + randomFloat(4, 9),
                                   current.z + randomFloat(-3, 3)});
        b.body->setAngularVelocity({randomFloat(-5, 5),
                                    randomFloat(-5, 5),
                                    randomFloat(-5, 5)});
    }
}

void World::explosion()
{
    auto cx = randomFloat(-12, 12);
    auto cy = 1.5f;
    auto cz = randomFloat(-12, 12);
    constexpr auto blastRadius = 4.0f;
    constexpr auto strength = 14.0f;

    for (auto& b: bodies)
    {
        if (b.id == 0)
            continue;
        if (b.body->getType() == rp::BodyType::STATIC)
            continue;

        auto pos = b.body->getTransform().getPosition();
        auto dx = (float) pos.x - cx;
        auto dy = (float) pos.y - cy;
        auto dz = (float) pos.z - cz;
        auto distSq = dx * dx + dy * dy + dz * dz;
        if (distSq > blastRadius * blastRadius)
            continue;

        auto dist = std::sqrt(distSq);
        if (dist < 0.001f)
            continue;

        auto falloff = 1.0f - dist / blastRadius;
        auto kick = strength * falloff;
        auto current = b.body->getLinearVelocity();
        b.body->setLinearVelocity(
            {current.x + (dx / dist) * kick,
             current.y + (dy / dist) * kick + 2.0f,
             current.z + (dz / dist) * kick});
    }
}

void World::buildScene()
{
    auto identity = rp::Transform::identity();

    auto groundExtents = rp::Vector3 {30, 0.5f, 30};
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

    constexpr auto cols = 30;
    constexpr auto rows = 5;
    constexpr auto layers = 30;
    constexpr auto half = 0.5f;
    constexpr auto spacing = half * 2.05f;
    constexpr auto layerStartZ = -((layers - 1) * spacing) * 0.5f;
    constexpr auto colStartX = -((cols - 1) * spacing) * 0.5f;

    auto boxShape = common.createBoxShape({half, half, half});

    for (auto layer = 0; layer < layers; ++layer)
        for (auto row = 0; row < rows; ++row)
            for (auto col = 0; col < cols; ++col)
            {
                auto x = colStartX + (float) col * spacing;
                auto y = half + (float) row * spacing;
                auto z = layerStartZ + (float) layer * spacing;

                auto transform =
                    rp::Transform {{x, y, z}, rp::Quaternion::identity()};
                auto* body = world->createRigidBody(transform);
                body->setType(rp::BodyType::DYNAMIC);
                body->addCollider(boxShape, identity);
                bodies.push_back(
                    {nextId++, body, false, 0, {half, half, half}});
            }

    for (auto i = 0; i < 100; ++i)
    {
        auto radius = 0.4f;
        auto x = randomFloat(-12, 12);
        auto y = 8.0f + (float) (i % 10) * 0.6f;
        auto z = randomFloat(-12, 12);

        auto shape = common.createSphereShape(radius);
        auto transform = rp::Transform {{x, y, z}, rp::Quaternion::identity()};
        auto* body = world->createRigidBody(transform);
        body->setType(rp::BodyType::DYNAMIC);
        body->addCollider(shape, identity);
        bodies.push_back({nextId++, body, true, radius, {}});
    }
}
