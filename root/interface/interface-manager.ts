import { ControlsController } from "./controls-controller";

export class InterfaceManager {
    private emscriptenModule: any;

    private controlsController: ControlsController;

    constructor(module: any) {
        this.controlsController = new ControlsController(module);
    }

    /*
    ** Controls Controller
    */
    public getControlsController(): ControlsController {
        return this.controlsController;
    }
}