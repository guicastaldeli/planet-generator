import { DocLoader } from "../../out/doc-loader.js";
export class ControlsController {
    constructor(module) {
        this.container = null;
        this.isVisible = true;
        this.emscriptenModule = module;
        this.loader = DocLoader.getInstance('./interface/_controls.html');
        this.init();
    }
    async init() {
        await this.extractContainer();
        await this.append();
        setTimeout(() => this.setupEventListeners(), 100);
    }
    async append() {
        if (this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendToDOM', null, ['string'], [html]);
        }
    }
    /**
     * Extract Container
     */
    async extractContainer() {
        try {
            const doc = await this.loader.load();
            if (!doc)
                throw new Error('doc err');
            this.container = doc.querySelector('.controls--container');
            return this.container;
        }
        catch (err) {
            console.error(err);
            return null;
        }
    }
    /**
     * Setup Event Listeners
     */
    setupEventListeners() {
        const container = document.querySelector('.controls--container');
        if (!container) {
            console.error('Container not found in DOM!');
            return;
        }
        container.querySelector('#control--actn')
            ?.addEventListener('click', () => {
            this.onControlsMenuClick?.();
            this.toggleControls();
        });
        container.querySelector('#control--actn-custom')
            ?.addEventListener('click', () => {
            this.onCustomPresetClick?.();
            this.emscriptenModule._showGenerator();
        });
        container.querySelector('#control--actn-save')
            ?.addEventListener('click', () => {
            this.onSavePresetClick?.();
            this.emscriptenModule._onSavePreset();
        });
        container.querySelector('#control--actn-import')
            ?.addEventListener('click', () => {
            this.onImportPresetClick?.();
        });
        container.querySelector('#control--actn-export')
            ?.addEventListener('click', () => {
            this.onExportPresetClick?.();
        });
        container.querySelector('#control--actn-reset')
            ?.addEventListener('click', () => {
            this.onResetToDefaultClick?.();
        });
        container.querySelector('#control--actn-clear')
            ?.addEventListener('click', () => {
            this.onClearClick?.();
        });
    }
    /**
     *
     * Actions
     *
     */
    showControls(show) {
        const controlList = document.querySelector('#control--list');
        if (!controlList) {
            console.warn('Control list not found in DOM');
            return;
        }
        if (window.getComputedStyle(controlList).display === "none") {
            controlList.style.display = "flex";
        }
        else {
            controlList.style.display = "none";
        }
        this.isVisible = show;
    }
    toggleControls() {
        this.isVisible = !this.isVisible;
        this.showControls(this.isVisible);
    }
    /**
     *
     * Callbacks
     *
     */
    onControlsMenu(cb) {
        this.onControlsMenuClick = cb;
    }
    onCustom(cb) {
        this.onCustomPresetClick = cb;
    }
    onSave(cb) {
        this.onSavePresetClick = cb;
    }
    onImport(cb) {
        this.onImportPresetClick = cb;
    }
    onExport(cb) {
        this.onExportPresetClick = cb;
    }
    onReset(cb) {
        this.onResetToDefaultClick = cb;
    }
    onClear(cb) {
        this.onClearClick = cb;
    }
    setupCallbacks() {
        this.onControlsMenu(() => {
            this.emscriptenModule._onControlsMenu();
        });
        this.onCustom(() => {
            this.emscriptenModule._onCustomPreset();
        });
        this.onSave(() => {
            this.emscriptenModule._onSavePreset();
        });
        this.onImport(() => {
            this.emscriptenModule._onImportPreset();
        });
        this.onExport(() => {
            this.emscriptenModule._onExportPreset();
        });
        this.onReset(() => {
            this.emscriptenModule._onResetToDefault();
        });
        this.onClear(() => {
            this.emscriptenModule._onClear();
        });
    }
}
