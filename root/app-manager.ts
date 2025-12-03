import { ControlsController } from "../out/interface/controls-controller.js";
import { GeneratorController } from "../out/generator-controller.js";
import { InfoController } from "../out/interface/info-controller.js";

export class AppManager {
    private controlsController: ControlsController;
    private infoController: InfoController;
    private generatorController: GeneratorController;

    constructor(module: any) {
        this.controlsController = new ControlsController(module);
        this.generatorController = new GeneratorController(module);
        this.infoController = new InfoController(module);
    }

    /*
    ** Controls Controller
    */
    public getControlsController(): ControlsController {
        return this.controlsController;
    }

    /*
    ** Info Controller
    */
    public getInfoController(): InfoController {
        return this.infoController;
    }

    /*
    ** Generator Controller
    */
    public getGeneratorController(): GeneratorController {
        return this.generatorController;
    }
}