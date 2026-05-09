export interface ResetResponse {
    ok: boolean;
}

export interface Vec3 {
    x: number;
    y: number;
    z: number;
}

export interface BodyDescriptor {
    id: number;
    isSphere: boolean;
    radius: number;
    halfExtents: Vec3;
}

export interface SceneSnapshot {
    bodies: BodyDescriptor[];
}

export interface LaunchRequest {
    origin: Vec3;
    direction: Vec3;
    speed: number;
}

export interface LaunchResponse {
    id: number;
}

export interface RainRequest {
    enabled: boolean;
}

export interface RainResponse {
    ok: boolean;
}

export interface Quat {
    x: number;
    y: number;
    z: number;
    w: number;
}

export interface BodyTransform {
    id: number;
    position: Vec3;
    rotation: Quat;
}

export interface WorldTick {
    time: number;
    bodies: BodyTransform[];
    removedIds: number[];
}

