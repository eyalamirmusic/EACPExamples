import type * as T from './schema';

export type Invoke = (command: string, payload: unknown) => Promise<unknown>;

export function makeBackend(invoke: Invoke)
{
    return {
        reset: (): Promise<T.ResetResponse> =>
            invoke('reset', {}) as Promise<T.ResetResponse>,
        launchBall: (req: T.LaunchRequest): Promise<T.LaunchResponse> =>
            invoke('launchBall', req) as Promise<T.LaunchResponse>,
    };
}
