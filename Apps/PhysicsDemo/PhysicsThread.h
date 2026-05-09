#pragma once

#include "Types.h"
#include "World.h"

#include <atomic>
#include <ea_data_structures/Structures/SharedGUIData.h>
#include <mutex>
#include <queue>
#include <thread>

// Owns the World and runs the physics simulation on a dedicated
// thread at ~120Hz. State out (per-tick + scene) goes through
// EA::RealTimeToGUI rings so the GUI thread can poll lock-free.
// Commands in (reset/launch/rain) come through a mutex-guarded queue
// drained at the start of each tick.
class PhysicsThread
{
public:
    static PhysicsThread& instance();

    // ---- GUI -> RT (called from the message thread) ----
    void enqueueReset();
    void enqueueLaunchBall(const LaunchRequest& request);
    void enqueueSetRain(bool enabled);

    // ---- RT -> GUI (called from the message thread) ----
    bool tryPullLatestTick(WorldTick& out);
    SceneSnapshot currentScene();

    PhysicsThread(const PhysicsThread&) = delete;
    PhysicsThread& operator=(const PhysicsThread&) = delete;

private:
    PhysicsThread();
    ~PhysicsThread();

    struct Command
    {
        enum class Kind
        {
            Reset,
            LaunchBall,
            SetRain
        };

        Kind kind = Kind::Reset;
        LaunchRequest launch = {};
        bool rainEnabled = false;
    };

    void runLoop();
    void drainCommands(bool& sceneDirty);

    World world = {};
    EA::RealTimeToGUI<WorldTick, 8> tickRing;
    EA::RealTimeToGUI<SceneSnapshot, 4> sceneRing;

    std::mutex commandMutex;
    std::queue<Command> pendingCommands;

    int lastTickFlagSeen = 0;
    int lastSceneFlagSeen = 0;
    SceneSnapshot cachedScene = {};

    std::atomic<bool> running {false};
    std::thread workerThread;
};
