import { DocumentLoader } from "../../out/document-loader.js";

export class ControlsController {
    private emscriptenModule: any;

    private loader: DocumentLoader;
    private container: HTMLElement | null = null;
    private isVisible: boolean = true;

    private onControlsMenuClick?: () => void;
    private onCustomPresetClick?: () => void;
    private onSavePresetClick?: () => void;
    private onImportPresetClick?: () => void;
    private onExportPresetClick?: () => void;
    private onResetToDefaultClick?: () => void;
    private onClearClick?: () => void;
    
    constructor(module: any) {
        this.emscriptenModule = module;
        this.loader = DocumentLoader.getInstance('./interface/_controls.html');
        this.init();
    }

    private async init(): Promise<void> {
        await this.extractContainer();
        await this.append();
        setTimeout(() => this.setupEventListeners(), 100);
    }

    private async append(): Promise<void> {
        if(this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendToDOM',
                null,
                ['string'],
                [html]
            );
        }
    }

    /*
    ** Extract Container
    */
    private async extractContainer(): Promise<HTMLElement | null> {
        try {
            const doc = await this.loader.load();
            if(!doc) throw new Error('doc err');

            this.container = doc.querySelector('.controls--container');
            return this.container;
        } catch(err) {
            console.error(err);
            return null;
        }
    }

    /*
    ** Setup Event Listeners
    */
    private setupEventListeners(): void {
        const container = document.querySelector('.controls--container');
        if(!container) {
            console.error('Container not found in DOM!');
            return;
        }

        container.querySelector('#control--actn')
            ?.addEventListener('click', () => {
                this.onControlsMenuClick?.();
                this.toggleControls();
            }
        );
        container.querySelector('#control--actn-custom')
            ?.addEventListener('click', () => {
                this.onCustomPresetClick?.();
                this.emscriptenModule._showGenerator();
            }
        );
        container.querySelector('#control--actn-save')
            ?.addEventListener('click', () => {
                this.onSavePresetClick?.();
                this.emscriptenModule._onSavePreset();
            }
        );
        container.querySelector('#control--actn-import')
            ?.addEventListener('click', () => {
                this.onImportPresetClick?.();
            }
        );
        container.querySelector('#control--actn-export')
            ?.addEventListener('click', () => {
                this.onExportPresetClick?.();
            }
        );
        container.querySelector('#control--actn-reset')
            ?.addEventListener('click', () => {
                this.onResetToDefaultClick?.();
            }
        );
        container.querySelector('#control--actn-clear')
            ?.addEventListener('click', () => {
                this.onClearClick?.();
            }
        );
    }

    /*
    **
    *** Actions
    **
    */
    private showControls(show: boolean): void {
        if(!this.container) throw new Error('container err');
        this.container.style.display = show ? 'block' : 'none';
        this.isVisible = show;
    }

    private toggleControls(): void {
        this.isVisible = !this.isVisible;
        this.showControls(this.isVisible);
    }

    /*
    **
    *** Callbacks
    **
    */
    public onControlsMenu(cb: () => void): void {
        this.onControlsMenuClick = cb;
    }

    public onCustom(cb: () => void): void {
        this.onCustomPresetClick = cb;
    }

    public onSave(cb: () => void): void {
        this.onSavePresetClick = cb;
    }

    public onImport(cb: () => void): void {
        this.onImportPresetClick = cb;
    }

    public onExport(cb: () => void): void {
        this.onExportPresetClick = cb;
    }

    public onReset(cb: () => void): void {
        this.onResetToDefaultClick = cb;
    }

    public onClear(cb: () => void): void {
        this.onClearClick = cb;
    }

    public setupCallbacks(): void {
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