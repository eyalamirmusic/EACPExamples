import type * as T from './schema';

export type Handlers = {
    reset(): T.ResetResponse | Promise<T.ResetResponse>;
    launchBall(req: T.LaunchRequest): T.LaunchResponse | Promise<T.LaunchResponse>;
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
        case 'launchBall': return await handlers.launchBall(payload as T.LaunchRequest);
        default: throw new UnknownCommandError(command);
    }
}
