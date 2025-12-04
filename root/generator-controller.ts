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
            console.log(res)
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
        if(!this.container || !this.options) {
            console.error('setOptions - missing container or options');
            return;
        }

        const domContainer = document.querySelector('#planet-creator-modal');
        console.log('setOptions - domContainer found:', !!domContainer);
        const workingContainer = domContainer || this.container;
        
        const shapeSelect = workingContainer.querySelector('#planet-shape') as HTMLSelectElement;
        console.log('setOptions - shapeSelect found:', !!shapeSelect);
        if(shapeSelect && this.options.generatorOptions.shapes) {
            shapeSelect.innerHTML = '';
            this.options.generatorOptions.shapes.forEach((shape: any) => {
                const option = document.createElement('option');
                option.value = shape.id;
                option.textContent = shape.name;
                shapeSelect.appendChild(option);
            });
            console.log('setOptions - shapes populated:', shapeSelect.children.length);
        }

        /* Rotation */
        const rotationSelect = workingContainer.querySelector('#rotation-axis') as HTMLSelectElement;
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
        const positionSelect = workingContainer.querySelector('#planet-position') as HTMLSelectElement;
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
        const sizeSlider = workingContainer.querySelector('#planet-size') as HTMLInputElement;
        if (sizeSlider && this.options.generatorOptions.sizeRange) {
            const range = this.options.generatorOptions.sizeRange;
            sizeSlider.min = (range.min * 100).toString();
            sizeSlider.max = (range.max * 100).toString();
            sizeSlider.value = (range.default * 100).toString();
            const sizeValue = workingContainer.querySelector('#size-value') as HTMLElement;
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
        const domContainer = document.querySelector('#planet-creator-modal');
        if(!domContainer) {
            console.error('container not found in DOM');
            return;
        }
        
        const nameInput = domContainer.querySelector('#planet-name') as HTMLInputElement;
        const shapeSelect = domContainer.querySelector('#planet-shape') as HTMLSelectElement;
        const sizeSlider = domContainer.querySelector('#planet-size') as HTMLInputElement;
        const colorInput = domContainer.querySelector('#planet-color') as HTMLInputElement;
        const positionSelect = domContainer.querySelector('#planet-position') as HTMLSelectElement;
        const rotationSelect = domContainer.querySelector('#rotation-axis') as HTMLSelectElement;
        const selfRotationSlider = domContainer.querySelector('#self-rotation') as HTMLInputElement;
        const orbitSlider = domContainer.querySelector('#orbit-speed') as HTMLInputElement;

        const data = {
            name: nameInput.value || `Planet ${Date.now()}`,
            shape: shapeSelect.value,
            size: parseInt(sizeSlider.value) / 100,
            color: colorInput.value,
            position: parseInt(positionSelect.value),
            rotationDir: rotationSelect.value,
            rotationSpeedItself: parseInt(selfRotationSlider.value) / 1000,
            rotationSpeedCenter: parseInt(orbitSlider.value) / 1000
        };
        
        const dataStr = JSON.stringify(data);
        if(
            this.emscriptenModule._malloc && 
            this.emscriptenModule.stringToUTF8 && 
            this.emscriptenModule.lengthBytesUTF8
        ) {
            console.log('Using manual memory allocation');
            try {
                const byteLength = this.emscriptenModule.lengthBytesUTF8(dataStr) + 1;
                console.log('Allocating', byteLength, 'bytes');
                
                const ptr = this.emscriptenModule._malloc(byteLength);
                console.log('Allocated at pointer:', ptr);
                
                if(ptr === 0) {
                    console.error('Failed to allocate memory');
                } else {
                    this.emscriptenModule.stringToUTF8(dataStr, ptr, byteLength);
                    console.log('String copied to WebAssembly memory');
                    
                    console.log('Calling _generatePlanetParser with pointer:', ptr);
                    this.emscriptenModule._generatePlanetParser(ptr);
                    
                    this.emscriptenModule._free(ptr);
                    console.log('Memory freed');
                    
                    console.log('Planet generation process completed');
                    (domContainer as HTMLElement).style.display = 'none';
                    return;
                }
            } catch (err) {
                console.error('Manual allocation failed:', err);
            }
        }
        if(this.emscriptenModule.ccall) {
            console.log('Using ccall');
            try {
                this.emscriptenModule.ccall('generatePlanetParser', 
                    null, 
                    ['string'], 
                    [dataStr]
                );
                console.log('Planet generation process completed');
                (domContainer as HTMLElement).style.display = 'none';
                return;
            } catch (err) {
                console.error('ccall failed:', err);
            }
        }
        console.error('No valid method found for planet generation');
    }

    /*
    ** Setup Event Listeners
    */
    public setupEventListeners(): void {
        const domContainer = document.querySelector('#planet-creator-modal');
        if(!domContainer) {
            console.error('setupEventListeners - modal not found in DOM');
            return;
        }

        const updatePreview = () => {
            const data = this.getCurrentData();
            if(this.emscriptenModule._updatePreviewPlanet) {
                this.emscriptenModule._updatePreviewPlanet(JSON.stringify(data));
            } else if(this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall('updatePreviewPlanet', 
                    null, 
                    ['string'], 
                    [JSON.stringify(data)]
                );
            }
        }

        let updateTimeout: NodeJS.Timeout;
        const debouncedUpdate = () => {
            clearTimeout(updateTimeout);
            updateTimeout = setTimeout(updatePreview, 100);
        }

        const formInputs = domContainer.querySelectorAll('input, select, range');
        formInputs.forEach(input => {
            input.addEventListener('input', debouncedUpdate);
            input.addEventListener('change', debouncedUpdate);
        });
        
        domContainer.querySelector('#create-planet-btn')?.addEventListener('click', () => {
            console.log('Create planet button clicked!');
            this.generate();
        });
        
        /* Size Slider */
        const sizeSlider = domContainer.querySelector('#planet-size') as HTMLInputElement;
        const sizeValue = domContainer.querySelector('#size-value') as HTMLElement;
        if(sizeSlider && sizeValue) {
            sizeSlider.addEventListener('input', () => {
                const value = parseInt(sizeSlider.value) / 100;
                sizeValue.textContent = value.toFixed(2);
            });
        }

        /* Self Rotation */
        const selfRotationSlider = domContainer.querySelector('#self-rotation') as HTMLInputElement;
        const selfRotationValue = domContainer.querySelector('#self-rotation-value') as HTMLElement;
        if(selfRotationSlider && selfRotationValue) {
            selfRotationSlider.addEventListener('input', () => {
                const value = parseInt(selfRotationSlider.value) / 1000;
                selfRotationValue.textContent = value.toFixed(3);
            });
        }
        
        /* Orbit Rotation */
        const orbitSlider = domContainer.querySelector('#orbit-speed') as HTMLInputElement;
        const orbitValue = domContainer.querySelector('#orbit-speed-value') as HTMLElement;
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
    ** Get Current Data
    */
    private getCurrentData(): any {
        const domContainer = document.querySelector('#planet-creator-modal');
        if(!domContainer) {
            console.log('no dom container!');
            return;
        }
        
        const nameInput = domContainer.querySelector('#planet-name') as HTMLInputElement;
        const shapeSelect = domContainer.querySelector('#planet-shape') as HTMLSelectElement;
        const sizeSlider = domContainer.querySelector('#planet-size') as HTMLInputElement;
        const colorInput = domContainer.querySelector('#planet-color') as HTMLInputElement;
        const rotationSelect = domContainer.querySelector('#rotation-axis') as HTMLSelectElement;
        const selfRotationSlider = domContainer.querySelector('#self-rotation') as HTMLInputElement;
        const orbitSlider = domContainer.querySelector('#orbit-speed') as HTMLInputElement;
        
        return {
            name: nameInput?.value || `Planet ${Date.now()}`,
            shape: shapeSelect?.value || 'SPHERE',
            size: sizeSlider ? parseInt(sizeSlider.value) / 100 : 1.0,
            color: colorInput?.value || '#808080',
            rotationDir: rotationSelect?.value || 'Y',
            rotationSpeedItself: selfRotationSlider ? parseInt(selfRotationSlider.value) / 1000 : 0.01,
            rotationSpeedCenter: orbitSlider ? parseInt(orbitSlider.value) / 1000 : 0.01
        };
    }

    /*
    ** Setup Callbacks
    */
    public setupCallbacks(): void {
        this.onGenerate((data: any) => {
            const dataStr = JSON.stringify(data);
            if(this.emscriptenModule._generatePlanetParser) {
                this.emscriptenModule._generatePlanetParser(dataStr);
            } else if(this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall('generatePlanetParser', null, ['string'], [dataStr]);
            } else {
                console.error('No valid Emscripten function found');
            }
        });
        this.onCancel(() => {
            this.emscriptenModule._hideGenerator();
        });
    }
}