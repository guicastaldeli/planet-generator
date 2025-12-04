import { DocumentLoader } from "../out/document-loader.js";

interface GeneratorOptions {
    shapes: Array<{ 
        id: string; 
        name: string; 
        description: string 
    }>;
    rotationAxes: Array<{ 
        id: string;
     name: string 
    }>;
    orbitPositions: Array<{ 
        id: number; 
        name: string; 
        distance: number 
    }>;
    sizeRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number 
    };
    rotationSpeedRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number 
    };
    orbitSpeedRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number 
    };
}

interface OptionsData {
    generatorOptions: GeneratorOptions;
}

export class GeneratorController {
    private emscriptenModule: any;
    
    private loader: DocumentLoader;
    private container: HTMLElement | null = null;
    private options: OptionsData | null = null;

    private onGenerateClick?: (data: any) => void;
    private onCancelClick?: () => void;

    constructor(module: any) {
        this.emscriptenModule = module;
        this.loader = DocumentLoader.getInstance('./interface/_generator-menu.html');
        this.init();
    }

    private async init(): Promise<void> {
        await this.loadOptions();
        await this.extractContainer();
        await this.append();
        await this.setOptions();
        setTimeout(() => this.setupEventListeners(), 100);
    }

    private async append(): Promise<void> {
        if(this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendGeneratorToDOM',
                null,
                ['string'],
                [html]
            );
        }
    }

    /*
    ** Load Options
    */
    private async loadOptions(): Promise<void> {
        try {
            const res = await fetch('./_data/options.json');
            if(!res.ok) {
                throw new Error(`ERROR!!: ${res.status}`);
            }
            this.options = await res.json();
        } catch(err) {
            console.error('Failed to load options', err);
        }
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

    /*
    ** Set Options
    */
    private async setOptions(): Promise<void> {
        if(!this.container || !this.options) return;

        /* Shape */
        const shapeSelect = this.container.querySelector('#planet-shape') as HTMLSelectElement;
        if(shapeSelect && this.options.generatorOptions.shapes) {
            shapeSelect.innerHTML = '';
            this.options.generatorOptions.shapes.forEach((shape: any) => {
                const option = document.createElement('option');
                option.value = shape.id;
                option.textContent = shape.name;
                shapeSelect.appendChild(option);
            });
        }

        /* Rotation */
        const rotationSelect = this.container.querySelector('#rotation-axis') as HTMLSelectElement;
        if(rotationSelect && this.options.generatorOptions.rotationAxes) {
            rotationSelect.innerHTML = '';
            this.options.generatorOptions.rotationAxes.forEach((axis: any) => {
                const option = document.createElement('option');
                option.value = axis.id;
                option.textContent = axis.name;
                if(axis.id === 'Y') option.selected = true;
                rotationSelect.appendChild(option);
            });
        }

        /* Position */
        const positionSelect = this.container.querySelector('#planet-position') as HTMLSelectElement;
        if(positionSelect && this.options.generatorOptions.orbitPositions) {
            positionSelect.innerHTML = '';
            this.options.generatorOptions.orbitPositions.forEach((orbit: any) => {
                const option = document.createElement('option');
                option.value = orbit.id.toString();
                option.textContent = orbit.name;
                positionSelect.appendChild(option);
            });
        }

        /* Size */
        const sizeSlider = this.container.querySelector('#planet-size') as HTMLInputElement;
        if (sizeSlider && this.options.generatorOptions.sizeRange) {
            const range = this.options.generatorOptions.sizeRange;
            sizeSlider.min = (range.min * 100).toString();
            sizeSlider.max = (range.max * 100).toString();
            sizeSlider.value = (range.default * 100).toString();
            const sizeValue = this.container.querySelector('#size-value') as HTMLElement;
            if(sizeValue) {
                sizeValue.textContent = range.default.toFixed(2);
            }
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
            console.error('extractContainer error:', err);
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
        this.emscriptenModule._generatePlanetParser(JSON.stringify(data));
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

    public onGenerate(cb: (data: any) => void): void {
        this.onGenerateClick = cb;
    }

    public onCancel(cb: () => void): void {
        this.onCancelClick = cb;
    }

    /*
    ** Setup Callbacks
    */
    public setupCallbacks(): void {
        this.onGenerate((data: any) => {
            this.emscriptenModule._generatePlanetParser(JSON.stringify(data));
        });
        this.onCancel(() => {
            this.emscriptenModule._hideGenerator();
        });
    }
}