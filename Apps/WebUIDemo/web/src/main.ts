import './style.css';
import { backend } from './generated/backend';

const root = document.querySelector<HTMLDivElement>('#app')!;

root.innerHTML = `
    <h1>EACP + Miro WebUI Demo</h1>
    <p>Native window, embedded WebView, typed RPC across the bridge.</p>
    <input id="name" placeholder="your name" />
    <button id="greet">Greet from native</button>
    <div id="out"></div>
`;

const out = root.querySelector<HTMLDivElement>('#out')!;
const button = root.querySelector<HTMLButtonElement>('#greet')!;
const nameInput = root.querySelector<HTMLInputElement>('#name')!;

button.addEventListener('click', async () => {
    out.textContent = 'calling native...';
    const data = await backend.greet({ name: nameInput.value });
    const serverTime = new Date(data.serverTimeMs).toLocaleTimeString();
    out.textContent = `${data.message} (server time: ${serverTime})`;
});
