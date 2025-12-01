import { AppController } from "./root/app-controller";

interface EmscriptenModule {
    onRuntimeInitialized?: () => void;
    [key: string]: any;
}

declare global {
    interface Window {
        AppController: typeof AppController;
        appController?: AppController;
        Module?: EmscriptenModule;
    }
}