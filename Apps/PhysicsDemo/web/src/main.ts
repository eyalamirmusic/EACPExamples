import './style.css';
import * as THREE from 'three';
import { backend } from './generated/backend';
import type { BodyState, WorldState } from './generated/schema';

const canvas = document.getElementById('scene') as HTMLCanvasElement;
const info = document.getElementById('info')!;
const resetButton = document.getElementById('reset') as HTMLButtonElement;

const renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
renderer.setPixelRatio(window.devicePixelRatio);
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setClearColor(0x1e1e28);

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 0.1, 200);
camera.position.set(9, 7, 12);
camera.lookAt(0, 2, -2);

scene.add(new THREE.AmbientLight(0xffffff, 0.45));
const sun = new THREE.DirectionalLight(0xffffff, 0.85);
sun.position.set(10, 20, 10);
scene.add(sun);
const fill = new THREE.DirectionalLight(0x8090ff, 0.25);
fill.position.set(-8, 6, -4);
scene.add(fill);

const meshes = new Map<number, THREE.Mesh>();
const sphereGeometry = new THREE.SphereGeometry(1, 18, 12);
const boxGeometry = new THREE.BoxGeometry(1, 1, 1);

function colorFor(id: number, isSphere: boolean): THREE.Color
{
    if (!isSphere && id === 0)
        return new THREE.Color(0x2a2a3a);
    const hue = ((id * 47) % 360) / 360;
    return new THREE.Color().setHSL(hue, isSphere ? 0.55 : 0.4, 0.55);
}

function meshFor(state: BodyState): THREE.Mesh
{
    const existing = meshes.get(state.id);
    if (existing) return existing;

    let mesh: THREE.Mesh;
    if (state.isSphere)
    {
        mesh = new THREE.Mesh(
            sphereGeometry,
            new THREE.MeshStandardMaterial({
                color: colorFor(state.id, true),
                roughness: 0.4,
            }));
        mesh.scale.setScalar(state.radius);
    }
    else
    {
        const e = state.halfExtents;
        mesh = new THREE.Mesh(
            boxGeometry,
            new THREE.MeshStandardMaterial({
                color: colorFor(state.id, false),
                roughness: state.id === 0 ? 0.95 : 0.6,
            }));
        mesh.scale.set(e.x * 2, e.y * 2, e.z * 2);
    }

    scene.add(mesh);
    meshes.set(state.id, mesh);
    return mesh;
}

let lastTickTime = performance.now();
let lastReportedTime = 0;
let smoothedHz = 60;

window.eacp.on<WorldState>('worldState', (state) => {
    const seen = new Set<number>();
    for (const body of state.bodies)
    {
        const mesh = meshFor(body);
        mesh.position.set(body.position.x, body.position.y, body.position.z);
        mesh.quaternion.set(
            body.rotation.x, body.rotation.y, body.rotation.z, body.rotation.w);
        seen.add(body.id);
    }

    for (const [id, mesh] of meshes)
    {
        if (!seen.has(id))
        {
            scene.remove(mesh);
            meshes.delete(id);
        }
    }

    const now = performance.now();
    const dt = now - lastTickTime;
    lastTickTime = now;
    if (dt > 0) smoothedHz = smoothedHz * 0.9 + (1000 / dt) * 0.1;

    if (state.time - lastReportedTime > 0.25)
    {
        info.textContent =
            `t=${state.time.toFixed(2)}s  bodies=${state.bodies.length}  ${smoothedHz.toFixed(0)} Hz`;
        lastReportedTime = state.time;
    }
});

resetButton.addEventListener('click', () => { void backend.reset(); });

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
        speed: 28,
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
