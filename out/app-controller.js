import { AppManager } from "../out/app-manager.js";
export class AppController {
    constructor(module) {
        this.appManager = new AppManager(module);
        this.setupCallbacks();
        this.exposeToEngine();
    }
    /**
     * Setup Callbacks
     */
    setupCallbacks() {
        this.appManager.getControlsController().setupCallbacks();
        this.appManager.getGeneratorController().setupCallbacks();
        this.appManager.getInfoController().setupCallbacks();
    }
    /**
     * Expose to Engine
     */
    exposeToEngine() {
        window.appController = this;
        this.getAppManager();
    }
    getAppManager() {
        return this.appManager;
    }
}
