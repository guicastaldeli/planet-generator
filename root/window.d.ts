import { AppController } from "./app-controller";

export {};

declare global {
    interface Window {
        AppController: typeof AppController;
        appController?: AppController;
    }
}