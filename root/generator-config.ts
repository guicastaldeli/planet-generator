export class GeneratorConfig {
    private data: any;

    constructor(data: any) {
        this.data = data;
    }

    public fieldMap = {
        'planet-name': {
            outputKey: 'name',
            transform: (value: string) => value || `Planet ${Date.now()}`
        },
        'planet-shape': {
            outputKey: 'shape',
            transform: (value: string) => value || 'SPHERE'
        },
        'planet-size': {
            outputKey: 'size',
            transform: (value: string) => Number(value)
        },
        'planet-color': {
            outputKey: 'color',
            transform: (value: string) => value || '#808080'
        },
        'planet-position': {
            outputKey: 'position',
            transform: (value: string) => Number(value)
        },
        'rotation-axis': {
            outputKey: 'rotationDir',
            transform: (value: string) => value || 'Y'
        },
        'self-rotation': {
            outputKey: 'rotationSpeedItself',
            transform: (value: string) => Number(value)
        },
        'orbit-speed': {
            outputKey: 'rotationSpeedCenter',
            transform: (value: string) => Number(value)
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
            defaultValue: '' 
        },
        { 
            id: 'planet-shape', 
            type: 'select', 
            defaultValue: 'SPHERE' 
        },
        { 
            id: 'planet-size', 
            type: 'range', 
            defaultValue: () => 
                this.data.generatorOptions.
                sizeRange.default 
        },
        { 
            id: 'planet-color', 
            type: 'color', 
            defaultValue: '#808080' 
        },
        { 
            id: 'planet-position', 
            type: 'select', 
            defaultValue: '1' 
        },
        { 
            id: 'rotation-axis', 
            type: 'select', 
            defaultValue: 'Y' 
        },
        { 
            id: 'self-rotation', 
            type: 'range', 
            defaultValue: () => 
                this.data.generatorOptions.
                rotationSpeedRange.default 
        },
        { 
            id: 'orbit-speed', 
            type: 'range', 
            defaultValue: () => 
                this.data.generatorOptions.
                orbitSpeedRange.default 
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
            default:
                return (el as HTMLInputElement).value || '';
        }
    }

    public idToCamelCase(id: string): string {
        if(id.startsWith('planet-')) {
            id = id.substring(7);
        }

        const regex = /-([a-z])/g;
        return id.replace(regex, (_, l) => l.toUpperCase());
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
    }

    private setupRangeValueDisplay(container: HTMLElement, id: string) {
        const slider = container?.querySelector(`#${id}`) as HTMLInputElement;
        const valueDisplayId = 'size-value';
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