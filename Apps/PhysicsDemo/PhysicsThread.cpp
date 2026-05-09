#include "PhysicsThread.h"
#include "WorldApi.h"

#include <chrono>

namespace
{
constexpr int kTargetHz = 120;
constexpr auto kTargetPeriod =
    std::chrono::nanoseconds {1'000'000'000 / kTargetHz};

// Push a fresh SceneSnapshot every ~0.5s so JS's getScene() cache
// catches up with bodies the chaos system has spawned. Reset and
// explicit launchBall pushes happen synchronously below.
constexpr int kScenePushEvery = 60;
} // namespace

PhysicsThread& PhysicsThread::instance()
{
    static PhysicsThread t;
    return t;
}

PhysicsThread::PhysicsThread()
{
    // Seed both rings before the worker starts so a getScene() before
    // the first tick still returns a populated snapshot.
    auto initialScene = world.sceneSnapshot();
    sceneRing.push(initialScene);
    cachedScene = initialScene;
    lastSceneFlagSeen = sceneRing.updateFlag.load();

    running.store(true);
    workerThread = std::thread {[this] { runLoop(); }};
}

PhysicsThread::~PhysicsThread()
{
    running.store(false);
    if (workerThread.joinable())
        workerThread.join();
}

void PhysicsThread::enqueueReset()
{
    auto cmd = Command {};
    cmd.kind = Command::Kind::Reset;
    auto lock = std::lock_guard {commandMutex};
    pendingCommands.push(cmd);
}

void PhysicsThread::enqueueLaunchBall(const LaunchRequest& request)
{
    auto cmd = Command {};
    cmd.kind = Command::Kind::LaunchBall;
    cmd.launch = request;
    auto lock = std::lock_guard {commandMutex};
    pendingCommands.push(cmd);
}

void PhysicsThread::enqueueSetRain(bool enabled)
{
    auto cmd = Command {};
    cmd.kind = Command::Kind::SetRain;
    cmd.rainEnabled = enabled;
    auto lock = std::lock_guard {commandMutex};
    pendingCommands.push(cmd);
}

bool PhysicsThread::tryPullLatestTick(WorldTick& out)
{
    auto flag = tickRing.updateFlag.load();
    if (flag == lastTickFlagSeen)
        return false;

    lastTickFlagSeen = flag;
    out = tickRing.pull();
    return true;
}

SceneSnapshot PhysicsThread::currentScene()
{
    auto flag = sceneRing.updateFlag.load();
    if (flag != lastSceneFlagSeen)
    {
        cachedScene = sceneRing.pull();
        lastSceneFlagSeen = flag;
    }
    return cachedScene;
}

void PhysicsThread::runLoop()
{
    auto previous = std::chrono::steady_clock::now();
    auto next = previous + kTargetPeriod;
    auto pushSceneCounter = 0;

    while (running.load())
    {
        std::this_thread::sleep_until(next);

        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration<float>(now - previous).count();
        previous = now;
        next += kTargetPeriod;

        // Cap dt so a sustained stall doesn't make rp3d explode with
        // an enormous step. 50ms == ~6x our target period.
        if (elapsed > 0.05f)
            elapsed = 0.05f;

        auto sceneDirty = false;
        drainCommands(sceneDirty);

        world.step(elapsed);

        tickRing.push(world.tickSnapshot());

        ++pushSceneCounter;
        if (sceneDirty || pushSceneCounter >= kScenePushEvery)
        {
            sceneRing.push(world.sceneSnapshot());
            pushSceneCounter = 0;
        }
    }
}

void PhysicsThread::drainCommands(bool& sceneDirty)
{
    auto local = std::queue<Command> {};
    {
        auto lock = std::lock_guard {commandMutex};
        std::swap(local, pendingCommands);
    }

    while (!local.empty())
    {
        const auto& cmd = local.front();
        switch (cmd.kind)
        {
            case Command::Kind::Reset:
                world.reset();
                sceneDirty = true;
                break;
            case Command::Kind::LaunchBall:
                world.launchBall(cmd.launch);
                sceneDirty = true;
                break;
            case Command::Kind::SetRain:
                world.setRainEnabled(cmd.rainEnabled);
                break;
        }
        local.pop();
    }
}

// ---- WorldApi free-function forwarders, now routed through the
// PhysicsThread instead of directly to a singleton World. ----

void resetWorld()
{
    PhysicsThread::instance().enqueueReset();
}

SceneSnapshot getSceneFromWorld()
{
    return PhysicsThread::instance().currentScene();
}

WorldTick snapshotTickFromWorld()
{
    auto tick = WorldTick {};
    PhysicsThread::instance().tryPullLatestTick(tick);
    return tick;
}

void stepWorld(float)
{
    // Physics is driven by the worker thread; nothing for the GUI to do.
}

int launchBallInWorld(const LaunchRequest& request)
{
    PhysicsThread::instance().enqueueLaunchBall(request);
    return 0;
}

void setRainInWorld(bool enabled)
{
    PhysicsThread::instance().enqueueSetRain(enabled);
}
