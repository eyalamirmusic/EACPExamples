#pragma once

#include "Types.h"

void resetWorld();
WorldState snapshotWorld();
void stepWorld(float timeStep);
int launchBallInWorld(const LaunchRequest& request);
