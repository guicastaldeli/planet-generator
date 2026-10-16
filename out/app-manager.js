import { ControlsController } from "../out/interface/controls-controller.js";
import { GeneratorController } from "../out/generator-controller.js";
import { InfoController } from "../out/interface/info-controller.js";
export class AppManager {
    constructor(module) {
        this.controlsController = new ControlsController(module);
        this.generatorController = new GeneratorController(module);
        this.infoController = new InfoController(module);
    }
    /**
     * Controls Controller
     */
    getControlsController() {
        return this.controlsController;
    }
    /**
     * Info Controller
     */
    getInfoController() {
        return this.infoController;
    }
    /**
     * Generator Controller
     */
    getGeneratorController() {
        return this.generatorController;
    }
}
