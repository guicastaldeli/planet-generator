export class GeneratorConfig {
    constructor(data, defaultData) {
        this.fileRegex = /\.[^/.]+$/;
        this.camelCaseRegex = /-([a-z])/g;
        this.fieldMap = {
            'planet-name': {
                outputKey: 'name',
                transform: (value) => value || this.defaultData.name
            },
            'planet-shape': {
                outputKey: 'shape',
                transform: (value) => value || this.defaultData.shape
            },
            'planet-size': {
                outputKey: 'size',
                transform: (value) => Number(value) || this.defaultData.size
            },
            'planet-color': {
                outputKey: 'color',
                transform: (value) => value || this.defaultData.color
            },
            'planet-texture': {
                outputKey: 'texture',
                transform: (value) => value || this.defaultData.texture
            },
            'planet-position': {
                outputKey: 'position',
                transform: (value) => Number(value) || this.defaultData.position
            },
            'planet-lightning': {
                outputKey: 'lightning',
                transform: (value) => value || this.defaultData.lightning || "None"
            },
            'planet-effects': {
                outputKey: 'effects',
                transform: (value) => value || this.defaultData.effects || "None"
            },
            'rotation-axis': {
                outputKey: 'rotationDir',
                transform: (value) => value || this.defaultData.rotationDir
            },
            'self-rotation': {
                outputKey: 'rotationSpeedItself',
                transform: (value) => Number(value) || this.defaultData.rotationSpeedItself
            },
            'orbit-speed': {
                outputKey: 'rotationSpeedCenter',
                transform: (value) => Number(value) || this.defaultData.rotationSpeedCenter
            }
        };
        this.options = [
            {
                id: 'planet-shape',
                type: 'select',
                dataPath: 'shapes',
                createOption: (item) => ({
                    value: item.id,
                    text: item.name,
                    selected: false
                })
            },
            {
                id: 'rotation-axis',
                type: 'select',
                dataPath: 'rotationAxes',
                createOption: (item) => ({
                    value: item.id,
                    text: item.name,
                    selected: item.id === 'Y'
                })
            },
            {
                id: 'planet-lightning',
                type: 'select',
                dataPath: 'lightning',
                createOption: (item) => ({
                    value: item.name,
                    text: item.name,
                    selected: false
                })
            },
            {
                id: 'planet-effects',
                type: 'select',
                dataPath: 'effects',
                createOption: (item) => ({
                    value: item.name,
                    text: item.name,
                    selected: false
                })
            },
            {
                id: 'planet-position',
                type: 'select',
                dataPath: 'orbitPositions',
                createOption: (item) => ({
                    value: item.id.toString(),
                    text: item.name,
                    selected: false
                })
            },
            {
                id: 'planet-size',
                type: 'range',
                dataPath: 'sizeRange',
                updateElement: (element, range) => {
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
                updateElement: (element, range) => {
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
                updateElement: (element, range) => {
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
        this.form = [
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
                id: 'planet-lightning',
                type: 'select',
                defaultValue: () => this.defaultData.lightning || "None"
            },
            {
                id: 'planet-effects',
                type: 'select',
                defaultValue: () => this.defaultData.effects || "None"
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
        this.data = data;
        this.defaultData = defaultData;
    }
    getProp(obj, path) {
        return obj[path];
    }
    setSelect(el, items, option) {
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
    getElementValue(el, type) {
        switch (type) {
            case 'text':
            case 'color':
                return el.value;
            case 'select':
                return el.value;
            case 'range':
                return el.value;
            case 'file':
                return el.files?.[0]?.name.replace(this.fileRegex, "") || '';
            default:
                return el.value || '';
        }
    }
    idToCamelCase(id) {
        if (id.startsWith('planet-')) {
            id = id.substring(7);
        }
        return id.replace(this.camelCaseRegex, (_, l) => l.toUpperCase());
    }
    setupTextureUpload(container, update) {
        const input = container.querySelector('#planet-texture');
        if (input) {
            input.addEventListener('change', (e) => {
                const file = e.target.files?.[0];
                if (file)
                    this.handleTextureUpload(file, update);
            });
        }
    }
    handleTextureUpload(file, update) {
        const reader = new FileReader();
        reader.onload = (e) => {
            const result = e.target?.result;
            const img = new Image();
            img.onload = () => {
                const name = file.name.replace(this.fileRegex, "");
                window.currentTextureData = {
                    name: name,
                    data: result,
                    width: img.width,
                    height: img.height
                };
                update();
            };
            img.src = result;
        };
    }
    setupFormElementListeners(container, update) {
        this.form.forEach(config => {
            const el = container.querySelector(`#${config.id}`);
            if (!el) {
                console.warn(`Element #${config.id} not found for event listener`);
                return;
            }
            el.addEventListener('input', update);
            el.addEventListener('change', update);
            if (config.type === 'range')
                this.setupRangeValueDisplay(container, config.id);
        });
        this.setupTextureUpload(container, update);
    }
    setupRangeValueDisplay(container, id) {
        const slider = container?.querySelector(`#${id}`);
        let valueDisplayId;
        if (id === 'planet-size') {
            valueDisplayId = 'size-value';
        }
        else {
            valueDisplayId = `${id}-value`;
        }
        const valueDisplay = container?.querySelector(`#${valueDisplayId}`);
        if (!slider || !valueDisplay) {
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
    setupButtonListeners(container, generate, cancelGeneration) {
        const createBtn = container?.querySelector('#create-planet-btn');
        const closeBtn = container?.querySelector('.close');
        const cancelBtn = container?.querySelector('#cancel-planet-btn');
        if (createBtn) {
            createBtn.addEventListener('click', () => generate());
        }
        if (closeBtn) {
            closeBtn.addEventListener('click', () => cancelGeneration());
        }
        if (cancelBtn) {
            cancelBtn.addEventListener('click', () => cancelGeneration());
        }
    }
    setLightning(data) {
        const option = this.data?.generatorOptions?.lightning.find((item) => item.name === data);
        return option ? option.id === 1 : false;
    }
    setEffects(data) {
        const option = this.data?.generatorOptions?.effects.find((item) => item.name === data);
        return option ? option.id : (this.defaultData.effects ?? 0);
    }
}
