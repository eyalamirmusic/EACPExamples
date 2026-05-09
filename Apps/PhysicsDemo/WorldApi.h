#pragma once

#include "Types.h"

void resetWorld();
SceneSnapshot getSceneFromWorld();
WorldTick snapshotTickFromWorld();
void stepWorld(float timeStep);
int launchBallInWorld(const LaunchRequest& request);
void setRainInWorld(bool enabled);
