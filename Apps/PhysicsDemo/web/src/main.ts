import './style.css';
import * as THREE from 'three';
import { backend } from './generated/backend';
import type {
    BodyDescriptor,
    SceneSnapshot,
    WorldTick,
} from './generated/schema';

const canvas = document.getElementById('scene') as HTMLCanvasElement;
const info = document.getElementById('info')!;
const resetButton = document.getElementById('reset') as HTMLButtonElement;
const rainButton = document.getElementById('rain') as HTMLButtonElement;

const renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
renderer.setPixelRatio(window.devicePixelRatio);
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setClearColor(0x1e1e28);

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 0.1, 400);
camera.position.set(28, 22, 36);
camera.lookAt(0, 2, 0);

scene.add(new THREE.AmbientLight(0xffffff, 0.45));
const sun = new THREE.DirectionalLight(0xffffff, 0.85);
sun.position.set(20, 40, 20);
scene.add(sun);
const fill = new THREE.DirectionalLight(0x8090ff, 0.25);
fill.position.set(-15, 8, -10);
scene.add(fill);

// Two big instance pools — one per primitive shape. Per-instance
// scale is encoded in each instance's matrix, so 1×1×1 base geometry
// is fine.
const SPHERE_CAP = 8000;
const BOX_CAP = 6000;

const sphereMesh = new THREE.InstancedMesh(
    new THREE.SphereGeometry(1, 12, 8),
    new THREE.MeshStandardMaterial({ roughness: 0.45 }),
    SPHERE_CAP);
sphereMesh.count = 0;
sphereMesh.instanceMatrix.setUsage(THREE.DynamicDrawUsage);
sphereMesh.instanceColor = new THREE.InstancedBufferAttribute(
    new Float32Array(SPHERE_CAP * 3), 3);
scene.add(sphereMesh);

const boxMesh = new THREE.InstancedMesh(
    new THREE.BoxGeometry(1, 1, 1),
    new THREE.MeshStandardMaterial({ roughness: 0.7 }),
    BOX_CAP);
boxMesh.count = 0;
boxMesh.instanceMatrix.setUsage(THREE.DynamicDrawUsage);
boxMesh.instanceColor = new THREE.InstancedBufferAttribute(
    new Float32Array(BOX_CAP * 3), 3);
scene.add(boxMesh);

interface Slot { isSphere: boolean; index: number; descriptor: BodyDescriptor; }

// id → which instance slot the body occupies
const slots = new Map<number, Slot>();
// id → last received transform (so we can re-stamp matrices each
// frame even if the body is sleeping and not in the latest tick)
const lastMatrix = new Map<number, THREE.Matrix4>();

let sphereCount = 0;
let boxCount = 0;

const tmpMatrix = new THREE.Matrix4();
const tmpPos = new THREE.Vector3();
const tmpQuat = new THREE.Quaternion();
const tmpScale = new THREE.Vector3();
const tmpColor = new THREE.Color();

function colorFor(id: number, isSphere: boolean, isGround: boolean): THREE.Color
{
    if (isGround) return tmpColor.set(0x2a2a3a);
    const hue = ((id * 47) % 360) / 360;
    return tmpColor.setHSL(hue, isSphere ? 0.55 : 0.4, isSphere ? 0.6 : 0.55);
}

function applyDescriptor(d: BodyDescriptor): Slot
{
    const isGround = d.id === 0;
    const slot: Slot = d.isSphere
        ? { isSphere: true, index: sphereCount++, descriptor: d }
        : { isSphere: false, index: boxCount++, descriptor: d };
    slots.set(d.id, slot);

    const color = colorFor(d.id, d.isSphere, isGround);
    if (slot.isSphere)
        sphereMesh.setColorAt(slot.index, color);
    else
        boxMesh.setColorAt(slot.index, color);

    // Initial matrix — at origin, identity rotation. Will be replaced
    // by the first tick that mentions this id; sleeping bodies never
    // get a tick so they sit at origin until we call getScene() again
    // with positions. (For this demo we resync via tick on every
    // movement, which is enough since reset wakes everything.)
    return slot;
}

function clearScene()
{
    slots.clear();
    lastMatrix.clear();
    sphereCount = 0;
    boxCount = 0;
}

async function syncScene()
{
    const snapshot: SceneSnapshot = await backend.getScene();
    clearScene();
    for (const desc of snapshot.bodies)
        applyDescriptor(desc);

    sphereMesh.count = sphereCount;
    boxMesh.count = boxCount;
    sphereMesh.instanceColor!.needsUpdate = true;
    boxMesh.instanceColor!.needsUpdate = true;
    sphereMesh.instanceMatrix.needsUpdate = true;
    boxMesh.instanceMatrix.needsUpdate = true;
    info.textContent = `bodies=${snapshot.bodies.length}  (waiting for first tick…)`;
}

let needsResync = false;
let lastResyncAt = 0;
let lastTickTime = performance.now();
let smoothedHz = 60;

window.eacp.on<WorldTick>('worldTick', (tick) => {
    for (const t of tick.bodies)
    {
        const slot = slots.get(t.id);
        if (!slot)
        {
            // Body the JS side hasn't seen — likely a launched ball or
            // rain sphere created on the C++ side. Throttle scene
            // re-fetches so a burst doesn't hammer the bridge.
            const now = performance.now();
            if (!needsResync && now - lastResyncAt > 250)
            {
                needsResync = true;
                lastResyncAt = now;
                void syncScene().finally(() => { needsResync = false; });
            }
            continue;
        }

        const d = slot.descriptor;
        tmpPos.set(t.position.x, t.position.y, t.position.z);
        tmpQuat.set(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w);
        if (d.isSphere)
            tmpScale.setScalar(d.radius);
        else
            tmpScale.set(d.halfExtents.x * 2, d.halfExtents.y * 2, d.halfExtents.z * 2);
        tmpMatrix.compose(tmpPos, tmpQuat, tmpScale);

        if (slot.isSphere)
            sphereMesh.setMatrixAt(slot.index, tmpMatrix);
        else
            boxMesh.setMatrixAt(slot.index, tmpMatrix);

        lastMatrix.set(t.id, tmpMatrix.clone());
    }

    sphereMesh.instanceMatrix.needsUpdate = true;
    boxMesh.instanceMatrix.needsUpdate = true;

    const now = performance.now();
    const dt = now - lastTickTime;
    lastTickTime = now;
    if (dt > 0) smoothedHz = smoothedHz * 0.92 + (1000 / dt) * 0.08;

    info.textContent =
        `t=${tick.time.toFixed(1)}s  total=${slots.size}  active=${tick.bodies.length}  ${smoothedHz.toFixed(0)}Hz`;
});

resetButton.addEventListener('click', async () => {
    await backend.reset();
    await syncScene();
});

let rainOn = false;
rainButton.addEventListener('click', async () => {
    rainOn = !rainOn;
    await backend.setRain({ enabled: rainOn });
    rainButton.classList.toggle('active', rainOn);
    rainButton.textContent = rainOn ? 'Rain: on' : 'Rain: off';
});

const raycaster = new THREE.Raycaster();
const ndc = new THREE.Vector2();

canvas.addEventListener('click', (event) => {
    const rect = canvas.getBoundingClientRect();
    ndc.set(
        ((event.clientX - rect.left) / rect.width) * 2 - 1,
        -((event.clientY - rect.top) / rect.height) * 2 + 1);
    raycaster.setFromCamera(ndc, camera);

    const dir = raycaster.ray.direction;
    const o = raycaster.ray.origin;
    void backend.launchBall({
        origin: { x: o.x, y: o.y, z: o.z },
        direction: { x: dir.x, y: dir.y, z: dir.z },
        speed: 35,
    });
});

window.addEventListener('resize', () => {
    renderer.setSize(window.innerWidth, window.innerHeight);
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
});

function render()
{
    requestAnimationFrame(render);
    renderer.render(scene, camera);
}
render();

// Bootstrap: ask C++ for the full body list, then let tick events
// drive transforms.
void syncScene();
