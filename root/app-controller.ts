import { InterfaceManager } from "../out/interface/interface-manager.js";

export class AppController {
    private interfaceManager: InterfaceManager;

    constructor(module?: any) {
        this.interfaceManager = new InterfaceManager(module);

        this.setupCallbacks();
        this.exposeToEngine();
    }

    /*
    ** Setup Callbacks
    */
    private setupCallbacks(): void {
        this.interfaceManager.getControlsController().setupCallbacks();
    }

    /*
    ** Expose to Engine
    */
    private exposeToEngine(): void {
        (window as any).appController = this;
    }
}