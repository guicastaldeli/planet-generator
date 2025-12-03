import { DocumentLoader } from "../out/document-loader.js";

export class GeneratorController {
    private emscriptenModule: any;
    
    private loader: DocumentLoader;
    private container: HTMLElement | null = null;

    constructor(module: any) {
        this.emscriptenModule = module;
        this.loader = DocumentLoader.getInstance('./interface/_generator-menu.html');
        this.init();
    }

    private hexToRgb(hex: string): { 
        r: number,
        g: number,
        b: number
    } {
        const res = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return res ? {
            r: parseInt(res[1], 16) / 255,
            g: parseInt(res[2], 16) / 255,
            b: parseInt(res[3], 16) / 255
        } :
        {
            r: 1.0,
            g: 0.5,
            b: 0.2
        }
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

            this.container = doc.querySelector('#planet-creator-modal');
            return this.container;
        } catch(err) {
            console.error(err);
            return null;
        }
    }

    /*
    ** Generate
    */
    private generate(): void {
        if(!this.container) return;
        
        const nameInput = this.container.querySelector('#planet-name') as HTMLInputElement;
        const shapeSelect = this.container.querySelector('#planet-shape') as HTMLSelectElement;
        const sizeSlider = this.container.querySelector('#planet-size') as HTMLInputElement;
        const colorInput = this.container.querySelector('#planet-color') as HTMLInputElement;
        const positionSelect = this.container.querySelector('#planet-position') as HTMLSelectElement;
        const rotationSelect = this.container.querySelector('#rotation-axis') as HTMLSelectElement;
        const selfRotationSlider = this.container.querySelector('#self-rotation') as HTMLInputElement;
        const orbitSlider = this.container.querySelector('#orbit-speed') as HTMLInputElement;
        
        const hexColor = colorInput.value;
        const rgb = this.hexToRgb(hexColor);

        const data = {
            name: nameInput.value || `Planet ${Date.now()}`,
            shape: shapeSelect.value,
            size: parseInt(sizeSlider.value) / 100,
            color: colorInput.value,
            rgbColor: rgb,
            position: parseInt(positionSelect.value),
            rotationDir: rotationSelect.value,
            rotationSpeedItself: parseInt(selfRotationSlider.value) / 1000,
            rotationSpeedCenter: parseInt(orbitSlider.value) / 1000
        };
        this.emscriptenModule._generate(JSON.stringify(data));
        if(this.container) {
            this.container.style.display = 'none';
        }
    }

    /*
    ** Setup Event Listeners
    */
    public setupEventListeners(): void {
        if(!this.container) return;
        this.container.querySelector('#create-planet-btn')?.addEventListener('click', () => {
            this.generate();
        });
        
        /* Size Slider */
        const sizeSlider = this.container.querySelector('#planet-size') as HTMLInputElement;
        const sizeValue = this.container.querySelector('#size-value') as HTMLElement;
        if(sizeSlider && sizeValue) {
            sizeSlider.addEventListener('input', () => {
                const value = parseInt(sizeSlider.value) / 100;
                sizeValue.textContent = value.toFixed(2);
            });
        }

        /* Self Rotation */
        const selfRotationSlider = this.container.querySelector('#self-rotation') as HTMLInputElement;
        const selfRotationValue = this.container.querySelector('#self-rotation-value') as HTMLElement;
        if(selfRotationSlider && selfRotationValue) {
            selfRotationSlider.addEventListener('input', () => {
                const value = parseInt(selfRotationSlider.value) / 1000;
                selfRotationValue.textContent = value.toFixed(3);
            });
        }
        
        /* Orbit Rotation */
        const orbitSlider = this.container.querySelector('#orbit-speed') as HTMLInputElement;
        const orbitValue = this.container.querySelector('#orbit-speed-value') as HTMLElement;
        if(orbitSlider && orbitValue) {
            orbitSlider.addEventListener('input', () => {
                const value = parseInt(orbitSlider.value) / 1000;
                orbitValue.textContent = value.toFixed(3);
            });
        }
    }
}