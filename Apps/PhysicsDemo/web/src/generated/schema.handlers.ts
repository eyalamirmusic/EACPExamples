import type * as T from './schema';

export type Handlers = {
    reset(): T.ResetResponse | Promise<T.ResetResponse>;
    getScene(): T.SceneSnapshot | Promise<T.SceneSnapshot>;
    launchBall(req: T.LaunchRequest): T.LaunchResponse | Promise<T.LaunchResponse>;
    setRain(req: T.RainRequest): T.RainResponse | Promise<T.RainResponse>;
};

export class UnknownCommandError extends Error
{
    httpStatus = 404;
    constructor(command: string)
    {
        super(`Unknown command: ${command}`);
    }
}

export async function dispatch(handlers: Handlers, command: string, payload: unknown): Promise<unknown>
{
    switch (command)
    {
        case 'reset': return await handlers.reset();
        case 'getScene': return await handlers.getScene();
        case 'launchBall': return await handlers.launchBall(payload as T.LaunchRequest);
        case 'setRain': return await handlers.setRain(payload as T.RainRequest);
        default: throw new UnknownCommandError(command);
    }
}
