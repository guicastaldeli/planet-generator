import { DocumentLoader } from "../document-loader";

export class ControlsController {
    private loader: DocumentLoader;
    private container: HTMLElement | null = null;
    private docLoaded: boolean = false;
    private isVisible: boolean = true;

    private onControlsMenuClick?: () => void;
    private onCustomPresetClick?: () => void;
    private onImportPresetClick?: () => void;
    private onExportPresetClick?: () => void;
    private onResetToDefaultClick?: () => void;
    
    constructor() {
        this.loader = DocumentLoader.getInstance('./_controls.html');
        this.extractContainer();
        this.setupEventListeners();
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
        if(!this.container) throw new Error('container err!');

        this.container.querySelector('#control--actn')
            ?.addEventListener('click', () => {
                this.onControlsMenuClick?.();
                this.toggleControls();
            }
        );
        this.container.querySelector('#control--actn-custom')
            ?.addEventListener('click', () => {
                this.onCustomPresetClick?.();
            }
        );
        this.container.querySelector('#control--actn-import')
            ?.addEventListener('click', () => {
                this.onImportPresetClick?.();
            }
        );
        this.container.querySelector('#control--actn-export')
            ?.addEventListener('click', () => {
                this.onExportPresetClick?.();
            }
        );
        this.container.querySelector('#control--actn-reset')
            ?.addEventListener('click', () => {
                this.onResetToDefaultClick?.();
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
    public onControlMenu(cb: () => void): void {
        this.onControlsMenuClick = cb;
    }
    
    public onCustom(cb: () => void): void {
        this.onControlsMenuClick = cb;
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
}