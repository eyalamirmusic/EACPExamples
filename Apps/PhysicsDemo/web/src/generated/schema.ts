export interface ResetResponse {
    ok: boolean;
}

export interface Vec3 {
    x: number;
    y: number;
    z: number;
}

export interface LaunchRequest {
    origin: Vec3;
    direction: Vec3;
    speed: number;
}

export interface LaunchResponse {
    id: number;
}

export interface Quat {
    x: number;
    y: number;
    z: number;
    w: number;
}

export interface BodyState {
    id: number;
    position: Vec3;
    rotation: Quat;
    halfExtents: Vec3;
    radius: number;
    isSphere: boolean;
}

export interface WorldState {
    time: number;
    bodies: BodyState[];
}

