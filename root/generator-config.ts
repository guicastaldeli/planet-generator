export class GeneratorConfig {
    private data: any;
    private defaultData: any;

    private fileRegex: RegExp = /\.[^/.]+$/;
    private camelCaseRegex: RegExp = /-([a-z])/g;

    constructor(data: any, defaultData: any) {
        this.data = data;
        this.defaultData = defaultData;
    }

    public fieldMap = {
        'planet-name': {
            outputKey: 'name',
            transform: (value: string) => value || this.defaultData.name
        },
        'planet-shape': {
            outputKey: 'shape',
            transform: (value: string) => value || this.defaultData.shape
        },
        'planet-size': {
            outputKey: 'size',
            transform: (value: string) => Number(value) || this.defaultData.size
        },
        'planet-color': {
            outputKey: 'color',
            transform: (value: string) => value || this.defaultData.color
        },
        'planet-texture': {
            outputKey: 'texture',
            transform: (value: string) => value || this.defaultData.texture
        },
        'planet-position': {
            outputKey: 'position',
            transform: (value: string) => Number(value) || this.defaultData.position
        },
        'rotation-axis': {
            outputKey: 'rotationDir',
            transform: (value: string) => value || this.defaultData.rotationDir
        },
        'self-rotation': {
            outputKey: 'rotationSpeedItself',
            transform: (value: string) => Number(value) || this.defaultData.rotationSpeedItself
        },
        'orbit-speed': {
            outputKey: 'rotationSpeedCenter',
            transform: (value: string) => Number(value) || this.defaultData.rotationSpeedCenter 
        }
    }
    public options = [
        {
            id: 'planet-shape',
            type: 'select',
            dataPath: 'shapes',
            createOption: (item: any) => ({
                value: item.id,
                text: item.name,
                selected: false
            })
        },
        {
            id: 'rotation-axis',
            type: 'select',
            dataPath: 'rotationAxes',
            createOption: (item: any) => ({
                value: item.id,
                text: item.name,
                selected: item.id === 'Y'
            })
        },
        {
            id: 'planet-position',
            type: 'select',
            dataPath: 'orbitPositions',
            createOption: (item: any) => ({
                value: item.id.toString(),
                text: item.name,
                selected: false
            })
        },
        {
            id: 'planet-size',
            type: 'range',
            dataPath: 'sizeRange',
            updateElement: (element: HTMLInputElement, range: any) => {
                element.min = range.min.toString();
                element.max = range.max.toString();
                element.step = range.step.toString();
                element.value = range.default.toString();
                
                const valElId = 'size-value';
                const valEl = document.getElementById(valElId);
                if (valEl) {
                    valEl.textContent = range.default.toFixed(2);
                }
            }
        },
        {
            id: 'self-rotation',
            type: 'range',
            dataPath: 'rotationSpeedRange',
            updateElement: (element: HTMLInputElement, range: any) => {
                element.min = range.min.toString();
                element.max = range.max.toString();
                element.step = range.step.toString();
                element.value = range.default.toString();
                
                const valElId = `${element.id}-value`;
                const valEl = document.getElementById(valElId);
                if (valEl) {
                    valEl.textContent = range.default.toFixed(3);
                }
            }
        },
        {
            id: 'orbit-speed',
            type: 'range',
            dataPath: 'orbitSpeedRange',
            updateElement: (element: HTMLInputElement, range: any) => {
                element.min = range.min.toString();
                element.max = range.max.toString();
                element.step = range.step.toString();
                element.value = range.default.toString();
                
                const valElId = `${element.id}-value`;
                const valEl = document.getElementById(valElId);
                if (valEl) {
                    valEl.textContent = range.default.toFixed(3);
                }
            }
        }
    ];
    public form = [
        { 
            id: 'planet-name', 
            type: 'text', 
            defaultValue: () => this.defaultData.name
        },
        { 
            id: 'planet-shape', 
            type: 'select', 
            defaultValue: () => this.defaultData.shape
        },
        { 
            id: 'planet-size', 
            type: 'range', 
            defaultValue: () => this.defaultData.size
        },
        { 
            id: 'planet-color', 
            type: 'color', 
            defaultValue: () => this.defaultData.color
        },
        { 
            id: 'planet-texture', 
            type: 'file', 
            defaultValue: () => this.defaultData.texture
        },
        { 
            id: 'planet-position', 
            type: 'select', 
            defaultValue: () => this.defaultData.position?.toString()
        },
        { 
            id: 'rotation-axis', 
            type: 'select', 
            defaultValue: () => this.defaultData.rotationDir
        },
        { 
            id: 'self-rotation', 
            type: 'range', 
            defaultValue: () => this.defaultData.rotationSpeedItself
        },
        { 
            id: 'orbit-speed', 
            type: 'range', 
            defaultValue: () => this.defaultData.rotationSpeedCenter
        }
    ];

    public getProp(obj: any, path: string): any {
        return obj[path];
    }

    public setSelect(
        el: HTMLSelectElement,
        items: Array<any>,
        option: (item: any) => ({
            value: string;
            text: string;
            selected: boolean
        })
    ): void {
        el.innerHTML = '';
        items.forEach(item => {
            const config = option(item);
            const optionEl = document.createElement('option');
            optionEl.value = config.value;
            optionEl.textContent = config.text;
            optionEl.selected = config.selected;
            el.appendChild(optionEl);
        });
    }

    public getElementValue(el: HTMLElement, type: string): string {
        switch(type) {
            case 'text':
            case 'color':
                return (el as HTMLInputElement).value;
            case 'select':
                return (el as HTMLSelectElement).value;
            case 'range':
                return (el as HTMLInputElement).value;
            case 'file':
                return (el as HTMLInputElement).files?.[0]?.name.replace(this.fileRegex, "") || '';
            default:
                return (el as HTMLInputElement).value || '';
        }
    }

    public idToCamelCase(id: string): string {
        if(id.startsWith('planet-')) {
            id = id.substring(7);
        }
        return id.replace(this.camelCaseRegex, (_, l) => l.toUpperCase());
    }

    private setupTextureUpload(container: HTMLElement, update: () => void): void {
        const input = container.querySelector('#planet-texture') as HTMLInputElement;
        
        if(input) {
            input.addEventListener('change', (e) => {
                const file = (e.target as HTMLInputElement).files?.[0];
                if(file) this.handleTextureUpload(file, update);
            });
        }
    }

    private handleTextureUpload(file: File, update: () => void): void {
        const reader = new FileReader();
        reader.onload = (e) => {
            const result = e.target?.result as string;
            const img = new Image();
            img.onload = () => {
                const name = file.name.replace(this.fileRegex, "");
                (window as any).currentTextureData = {
                    path: name,
                    data: result,
                    width: img.width,
                    height: img.height
                }
                update();
            }
            img.src = result;
        }
    }

    public setupFormElementListeners(container: HTMLElement, update: () => void): void {
        this.form.forEach(config => {
            const el = container.querySelector(`#${config.id}`);
            if(!el) {
                console.warn(`Element #${config.id} not found for event listener`);
                return;
            }

            el.addEventListener('input', update);
            el.addEventListener('change', update);
            if(config.type === 'range') this.setupRangeValueDisplay(container, config.id);
        });
        this.setupTextureUpload(container, update);
    }

    private setupRangeValueDisplay(container: HTMLElement, id: string) {
        const slider = container?.querySelector(`#${id}`) as HTMLInputElement;
        let valueDisplayId: any;
        if(id === 'planet-size') {
            valueDisplayId = 'size-value';
        } else {
            valueDisplayId = `${id}-value`;
        }
        const valueDisplay = container?.querySelector(`#${valueDisplayId}`) as HTMLElement;
        if(!slider || !valueDisplay) {
            console.warn(`Slider or value display not found for ${id}`);
            return;
        }

        const precision = id.includes('rotation') ? 3 : 2;
        valueDisplay.textContent = Number(slider.value).toFixed(precision);
        slider.addEventListener('input', () => {
            const val = Number(slider.value);
            valueDisplay.textContent = val.toFixed(precision);
        });
    }

    public setupButtonListeners(
        container: HTMLElement, 
        generate: () => void, 
        cancelGeneration: () => void
    ): void {
        const createBtn = container?.querySelector('#create-planet-btn');
        const closeBtn = container?.querySelector('.close');
        const cancelBtn = container?.querySelector('#cancel-planet-btn');

        if(createBtn) {
            createBtn.addEventListener('click', () => generate());
        }
        if(closeBtn) {
            closeBtn.addEventListener('click', () => cancelGeneration());
        }
        if(cancelBtn) {
            cancelBtn.addEventListener('click', () => cancelGeneration());
        }
    }
}