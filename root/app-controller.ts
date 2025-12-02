import { AppManager } from "../out/app-manager.js";

export class AppController {
    private appManager: AppManager;

    constructor(module?: any) {
        this.appManager = new AppManager(module);
        this.setupCallbacks();
        this.exposeToEngine();
    }

    /*
    ** Setup Callbacks
    */
    private setupCallbacks(): void {
        this.appManager.getControlsController().setupCallbacks();
        this.appManager.getGeneratorController().setupCallbacks();
    }

    /*
    ** Expose to Engine
    */
    private exposeToEngine(): void {
        (window as any).appController = this;
    }
}