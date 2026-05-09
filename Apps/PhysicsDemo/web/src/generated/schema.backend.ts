import type * as T from './schema';

export type Invoke = (command: string, payload: unknown) => Promise<unknown>;

export function makeBackend(invoke: Invoke)
{
    return {
        reset: (): Promise<T.ResetResponse> =>
            invoke('reset', {}) as Promise<T.ResetResponse>,
        getScene: (): Promise<T.SceneSnapshot> =>
            invoke('getScene', {}) as Promise<T.SceneSnapshot>,
        launchBall: (req: T.LaunchRequest): Promise<T.LaunchResponse> =>
            invoke('launchBall', req) as Promise<T.LaunchResponse>,
        setRain: (req: T.RainRequest): Promise<T.RainResponse> =>
            invoke('setRain', req) as Promise<T.RainResponse>,
    };
}
