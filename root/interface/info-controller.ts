import { DocumentLoader } from "../../out/document-loader.js";

export class InfoController {
    private emscriptenModule: any;
    
    private loader: DocumentLoader;
    private container: HTMLElement | null = null;
        
    constructor(module: any) {
        this.emscriptenModule = module;
        this.loader = DocumentLoader.getInstance('./interface/_info.html');
        this.init();
    }

    private async init(): Promise<void> {
        await this.extractContainer();
        await this.append();
        this.setupEventListeners();
    }

    private async append(): Promise<void> {
        if(this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendInfoToDOM',
                null,
                ['string'],
                [html]
            );
        }
    }

    /*
    ** Display Info
    */
    public display(name: string, info: string): void {
        const domContainer = document.querySelector('.info--container') as HTMLDivElement;
        this.container = domContainer;
        if(!domContainer) {
            console.error('Info container not found');
            return;
        }

        const actnsEl = domContainer.querySelector('#info--actns') as HTMLDivElement;
        if(actnsEl) actnsEl.style.display = 'block';
        
        const nameEl = domContainer.querySelector('#info--obj-name p');
        const infoEl = domContainer.querySelector('#info--obj-info p');
        if(nameEl && infoEl) {
            nameEl.textContent = name;
            infoEl.textContent = info;
        } else {
            console.error('Couldnt not find elements');
        }
    }

    /*
    ** Extract Container
    */
    private async extractContainer(): Promise<HTMLElement | null> {
        try {
            const doc = await this.loader.load();
            if(!doc) throw new Error('doc err');

            this.container = doc.querySelector('.info--container');
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
        setTimeout(() => {
            const domContainer = document.querySelector('.info--container') as HTMLDivElement;
            this.container = domContainer;

            const closeButton = domContainer.querySelector('#info--close-actn');
            const deleteButton = domContainer.querySelector('#info--delete-actn');
            
            if(closeButton) {
                closeButton.addEventListener('click', (e) => {
                    e.stopPropagation();
                    e.stopImmediatePropagation();

                    if(this.emscriptenModule._closeMenu) {
                        this.emscriptenModule._closeMenu();
                    } else if(this.emscriptenModule.ccall) {
                        this.emscriptenModule.ccall(
                            'closeMenu',
                            null,
                            [],
                            []
                        );
                    }
                });
            }
            if(deleteButton) {
                deleteButton.addEventListener('click', (e) => {
                    e.stopPropagation();
                    e.stopImmediatePropagation();

                    if(this.emscriptenModule._deletePlanet) {
                        this.emscriptenModule._deletePlanet();
                    } else if(this.emscriptenModule.ccall) {
                        this.emscriptenModule.ccall(
                            'deletePlanet',
                            null,
                            [],
                            []
                        );
                    }
                });
            }
        }, 100);
    }

    public setupCallbacks(): void {
        (window as any).display = (name: string, info: string) => {
            this.display(name, info);
        }
    }
}