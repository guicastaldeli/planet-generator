import { ControlsController } from "../out/interface/controls-controller.js";
import { GeneratorController } from "../out/generator-controller.js";

export class AppManager {
    private controlsController: ControlsController;
    private generatorController: GeneratorController;

    constructor(module: any) {
        this.controlsController = new ControlsController(module);
        this.generatorController = new GeneratorController(module);
    }

    /*
    ** Controls Controller
    */
    public getControlsController(): ControlsController {
        return this.controlsController;
    }

    /*
    ** Generator Controller
    */
    public getGeneratorController(): GeneratorController {
        return this.generatorController;
    }
}