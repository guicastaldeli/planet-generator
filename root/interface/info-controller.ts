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

    public setupCallbacks(): void {
        (window as any).display = (name: string, info: string) => {
            this.display(name, info);
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

        const nameEl = domContainer.querySelector('#info--obj-name p');
        const infoEl = domContainer.querySelector('#info--obj-info p');
        if(nameEl && infoEl) {
            nameEl.textContent = name;
            infoEl.textContent = info;
        } else {
            console.error('Couldm not find elements');
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

    private async init(): Promise<void> {
        await this.extractContainer();
        await this.append();
    }
}