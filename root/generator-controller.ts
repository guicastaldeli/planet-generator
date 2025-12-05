import { DocumentLoader } from "../out/document-loader.js";
import { GeneratorConfig } from "./generator-config.js";

interface GeneratorOptions {
    shapes: Array<{ 
        id: string; 
        name: string; 
        description: string;
    }>
    rotationAxes: Array<{ 
        id: string;
        name: string;
    }>
    orbitPositions: Array<{ 
        id: number; 
        name: string; 
        distance: number;
    }>
    sizeRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number;
    }
    rotationSpeedRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number;
    }
    orbitSpeedRange: { 
        min: number; 
        max: number; 
        step: number; 
        default: number;
    }
}

interface OptionsData {
    generatorOptions: GeneratorOptions;
}

export class GeneratorController {
    private emscriptenModule: any;
    
    private loader: DocumentLoader;
    private container: HTMLElement | null = null;
    private generatorConfig: GeneratorConfig | null = null;
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
        this.generatorConfig = new GeneratorConfig(this.options);
        
        await this.extractContainer();
        await this.append();
        await this.setOptions();
        
        if(!this.container) throw new Error('FATAL ERR container');
        this.container = document.querySelector('#planet-creator-modal');
        
        this.setupEventListeners();
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

    /*
    ** Get Data
    */
    private getData(): any {
        if(!this.container) {
            console.log('no dom container!');
            return;
        }

        const data: any = {};
        this.generatorConfig!.form.forEach(config => {
            const el = this.container!.querySelector(`#${config.id}`) as HTMLInputElement | HTMLSelectElement;
            if(!el) return;

            let val: any;
            switch(config.type) {
                case 'text':
                case 'color':
                    val = (el as HTMLInputElement).value;
                    break;
                case 'select':
                    val = (el as HTMLSelectElement).value;
                    break;
                case 'range':
                    val = Number((el as HTMLInputElement).value);
                    break;
            }

            let key = config.id;
            if(key.startsWith('planet-')) {
                key = key.substring(7);
            }

            const regex = /-([a-z])/g
            key = key.replace(regex, (_, l) => l.toUpperCase());

            data[key] = val;
        });
        return data;
    }

    /*
    ** Set Options
    */
    private async setOptions(): Promise<void> {
        const domContainer = document.querySelector('#planet-creator-modal') || this.container;
        if(!domContainer || !this.options || !this.generatorConfig) {
            console.error('set options err');
            return;
        }
    
        this.generatorConfig.options.forEach(config => {
            const el = domContainer.querySelector(`#${config.id}`) as HTMLElement;
            if(!el) {
                console.error(`Element #${config.id} not found`);
                console.log('Available elements in container:', 
                    Array.from(domContainer.querySelectorAll('*[id]')).map(el => el.id));
                return;
            }

            const data = this.generatorConfig!.getProp(
                this.options!.generatorOptions, 
                config.dataPath
            );
            if(!data) {
                console.error('data err');
                return;
            }

            switch(config.type) {
                case 'select':
                    this.generatorConfig!.setSelect(
                        el as HTMLSelectElement, 
                        data, 
                        config.createOption!
                    );
                    break;
                case 'range':
                    if(config.updateElement) {
                        config.updateElement(el as HTMLInputElement, data);
                    }
                    break;
                default:
                    console.error('ERRR');
            }
        });
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
        if(!this.container) {
            console.error('container not found in DOM');
            return;
        }
        
        const data = this.getData();
        const dataObj = {
            name: data.name,
            shape: data.shape,
            size: data.size,
            color: data.color,
            position: Number(data.position),
            rotationDir: data.rotationAxis,
            rotationSpeedItself: data.selfRotation,
            rotationSpeedCenter: data.orbitSpeed
        };
        const dataStr = JSON.stringify(dataObj);

        if(
            this.emscriptenModule._malloc && 
            this.emscriptenModule.stringToUTF8 && 
            this.emscriptenModule.lengthBytesUTF8
        ) {
            try {
                const byteLength = this.emscriptenModule.lengthBytesUTF8(dataStr) + 1;
                const ptr = this.emscriptenModule._malloc(byteLength);
                
                if(ptr === 0) {
                    console.error('Failed to allocate memory');
                } else {
                    this.emscriptenModule.stringToUTF8(dataStr, ptr, byteLength);
                    this.emscriptenModule._generatePlanetParser(ptr);
                    this.emscriptenModule._free(ptr);
                    (this.container as HTMLElement).style.display = 'none';
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
                (this.container as HTMLElement).style.display = 'none';
                return;
            } catch (err) {
                console.error('ccall failed:', err);
            }
        }
    }

    /*
    ** Cancel Generation
    */
    private cancelGeneration(): void {
        if(this.container) {
            (this.container as HTMLElement).style.display = 'none';
        }

        if(this.emscriptenModule._cleanupPreview) {
            this.emscriptenModule._cleanupPreview();
        } else if(this.emscriptenModule.ccall) {
            this.emscriptenModule.ccall(
                'cleanupPreview',
                null,
                [],
                []
            );
        }
    }

    /*
    ** Setup Event Listeners
    */
    public setupEventListeners(): void {
        if(!this.container) {
            console.error('setupEventListeners - modal not found in DOM');
            return;
        }

        const updatePreview = () => {
            const data = this.getCurrentData();
            const dataStr = JSON.stringify(data);
            if(typeof window['updatePreviewPlanet'] === 'function') {
                window['updatePreviewPlanet'](dataStr);
                return;
            }
            if(this.emscriptenModule && this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall(
                    'updatePreviewPlanet', 
                    'null', 
                    ['string'], 
                    [dataStr]
                );
                return;
            }
            if(this.emscriptenModule && this.emscriptenModule._updatePreviewPlanet) {
                const lengthBytes = this.emscriptenModule.lengthBytesUTF8(dataStr) + 1;
                const ptr = this.emscriptenModule._malloc(lengthBytes);
                this.emscriptenModule.stringToUTF8(dataStr, ptr, lengthBytes);
                this.emscriptenModule._updatePreviewPlanet(ptr);
                this.emscriptenModule._free(ptr);
                return;
            }
        }

        let updateTimeout: NodeJS.Timeout;
        const debouncedUpdate = () => {
            clearTimeout(updateTimeout);
            updateTimeout = setTimeout(updatePreview, 100);
        }

        this.generatorConfig!.form.forEach(config => {
            const el = this.container!.querySelector(`#${config.id}`);
            if(el) {
                el.addEventListener('input', debouncedUpdate);
                el.addEventListener('change', debouncedUpdate);
                if(config.type) {
                    el.addEventListener('input', () => {
                        const valEl = this.container!.querySelector(`#${config.id}-value`) as HTMLElement;
                        if(valEl) {
                            const precision = config.id.includes('rotation') ? 3 : 2;
                            valEl.textContent = Number((el as HTMLInputElement).value).toFixed(precision);
                        }
                    });
                }
            }
        })
        
        this.container.querySelector('#create-planet-btn')
            ?.addEventListener('click', () => {
                this.generate();
            }
        );
        this.container.querySelector('.close')
            ?.addEventListener('click', () => {
                this.cancelGeneration();
            }
        );
        this.container.querySelector('#cancel-planet-btn')
            ?.addEventListener('click', () => {
                this.cancelGeneration();
            }
        );
        
        /* Size Slider */
        const sizeSlider = this.container.querySelector('#planet-size') as HTMLInputElement;
        const sizeValue = this.container.querySelector('#size-value') as HTMLElement;
        if(sizeSlider && sizeValue) {
            sizeSlider.addEventListener('input', () => {
                const value = Number(sizeSlider.value);
                sizeValue.textContent = value.toFixed(2);
            });
        }

        /* Self Rotation */
        const selfRotationSlider = this.container.querySelector('#self-rotation') as HTMLInputElement;
        const selfRotationValue = this.container.querySelector('#self-rotation-value') as HTMLElement;
        if(selfRotationSlider && selfRotationValue) {
            selfRotationSlider.addEventListener('input', () => {
                const value = Number(selfRotationSlider.value);
                selfRotationValue.textContent = value.toFixed(3);
            });
        }
        
        /* Orbit Rotation */
        const orbitSlider = this.container.querySelector('#orbit-speed') as HTMLInputElement;
        const orbitValue = this.container.querySelector('#orbit-speed-value') as HTMLElement;
        if(orbitSlider && orbitValue) {
            orbitSlider.addEventListener('input', () => {
                const value = Number(orbitSlider.value);
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
    ** Reset Form Values
    */
    private resetFormValues(): void {
        if(!this.container) return;

        this.generatorConfig!.form.forEach(config => {
            const el = this.container!.querySelector(`#${config.id}`) as HTMLInputElement | HTMLSelectElement;
            if(!el) return;

            let defaultVal: any;
            if(typeof config.defaultValue === 'function') {
                defaultVal = config.defaultValue();
            } else  {
                defaultVal = config.defaultValue;
            }

            switch(config.type) {
                case 'text':
                case 'color':
                    (el as HTMLInputElement).value = defaultVal;
                    break;
                case 'select':
                    const select = el as HTMLSelectElement;
                    for(let i = 0; i < select.options.length; i++) {
                        if(select.options[i].value === defaultVal) {
                            select.selectedIndex = i;
                            break;
                        }
                    }
                    break;
                case 'range':
                    const range = el as HTMLInputElement;
                    range.value = defaultVal.toString();
                    const valEl = this.container!.querySelector(`#${config.id}-value`) as HTMLElement;
                    if(valEl) {
                        const precision = config.id.includes('rotation') ? 3 : 2;
                        valEl.textContent = Number(range.value).toFixed(precision);
                    }
                    break;
            }
        });
    }

    /*
    ** Get Current Data
    */
    private getCurrentData(): any {
        if(!this.container || !this.generatorConfig) {
            console.log('no container or config!');
            return {};
        }

        const data: any = {};
        this.generatorConfig.form.forEach(config => {
            const el = this.container!.querySelector(`#${config.id}`) as HTMLInputElement | HTMLSelectElement;
            if(!el) {
                console.warn(`Element #${config.id} not found`);
                return;
            }

            const val = this.generatorConfig?.getElementValue(el, config.type);
            const map = this.generatorConfig?.fieldMap[config.id as keyof typeof this.generatorConfig.fieldMap];
            if(map) {
                data[map.outputKey] = map.transform(val!);
            } else {
                const key = this.generatorConfig?.idToCamelCase(config.id);
                data[key!] = val;
            }
        });

        return data;
    }

    /*
    ** Show Generator
    */
    public showGenerator(): void {
        if(this.container) {
            (this.container as HTMLElement).style.display = 'block';
            this.resetFormValues();
        }
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
            if(this.emscriptenModule._cleanupPreview) {
                this.emscriptenModule._cleanupPreview();
            } else if(this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall(
                    'cleanupPreview',
                    null,
                    [],
                    []
                );
                this.emscriptenModule.ccall(
                    'hideGenerator',
                    null,
                    [],
                    []
                );
            } else if(this.emscriptenModule._hideGenerator) {
                this.emscriptenModule._hideGenerator();
            }
        });
    }
}