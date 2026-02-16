import { DocLoader } from "../out/doc-loader.js";
import { GeneratorConfig } from "./generator-config.js";
export class GeneratorController {
    constructor(module) {
        this.container = null;
        this.generatorConfig = null;
        this.options = null;
        this.defaultData = {};
        this.emscriptenModule = module;
        this.loader = DocLoader.getInstance('./interface/_generator-menu.html');
        this.init();
    }
    async init() {
        await this.loadOptions();
        await this.getDefaultData();
        this.generatorConfig = new GeneratorConfig(this.options, this.defaultData);
        await this.extractContainer();
        await this.append();
        await this.setOptions();
        if (!this.container)
            throw new Error('FATAL ERR container');
        this.container = document.querySelector('#planet-creator-modal');
        this.setupEventListeners();
    }
    async append() {
        if (this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendGeneratorToDOM', null, ['string'], [html]);
            return new Promise(res => {
                setTimeout(() => {
                    res();
                }, 100);
            });
        }
    }
    /**
     * Load Options
     */
    async loadOptions() {
        try {
            const res = await fetch('./_data/options.json');
            if (!res.ok) {
                throw new Error(`ERROR!!: ${res.status}`);
            }
            this.options = await res.json();
        }
        catch (err) {
            console.error('Failed to load options', err);
        }
    }
    /**
     * Get Default Data
     */
    async getDefaultData() {
        try {
            let dataStr = "";
            if (this.emscriptenModule._getDefaultData) {
                const ptr = this.emscriptenModule._getDefaultData();
                dataStr = this.emscriptenModule.UTF8ToString(ptr);
                console.log('Got default data from C++:', dataStr);
            }
            else if (this.emscriptenModule.ccall) {
                dataStr = this.emscriptenModule.ccall('getDefaultData', 'string', [], []);
                console.log('Got default data via ccall:', dataStr);
            }
            else if (typeof window['getDefaultData'] === 'function') {
                dataStr = window['getDefaultData']();
                console.log('Got default data from window:', dataStr);
            }
            if (dataStr && dataStr.trim() !== '{}') {
                try {
                    this.defaultData = JSON.parse(dataStr);
                    console.log('Parsed default data from C++:', this.defaultData);
                }
                catch (parseErr) {
                    console.error('Failed to parse default data JSON:', parseErr);
                }
            }
            else {
                console.warn('Default data is empty or invalid, using fallback defaults');
            }
        }
        catch (err) {
            console.error('Failed to load default planet data from C++:', err);
        }
    }
    /**
     * Set Options
     */
    async setOptions() {
        const domContainer = document.querySelector('#planet-creator-modal') || this.container;
        if (!domContainer || !this.options || !this.generatorConfig) {
            console.error('set options err');
            return;
        }
        this.generatorConfig.options.forEach(config => {
            const el = domContainer.querySelector(`#${config.id}`);
            if (!el) {
                console.error(`Element #${config.id} not found`);
                console.log('Available elements in container:', Array.from(domContainer.querySelectorAll('*[id]')).map(el => el.id));
                return;
            }
            const data = this.generatorConfig.getProp(this.options.generatorOptions, config.dataPath);
            if (!data) {
                console.error('data err');
                return;
            }
            switch (config.type) {
                case 'select':
                    this.generatorConfig.setSelect(el, data, config.createOption);
                    break;
                case 'range':
                    if (config.updateElement) {
                        config.updateElement(el, data);
                    }
                    break;
                default:
                    console.error('ERRR');
            }
        });
    }
    /**
     * Extract Container
     */
    async extractContainer() {
        try {
            const doc = await this.loader.load();
            if (!doc)
                throw new Error('doc err');
            this.container = doc.querySelector('#planet-creator-modal');
            return this.container;
        }
        catch (err) {
            console.error('extractContainer error:', err);
            return null;
        }
    }
    /**
     * Upload Texture
     */
    uploadTexture(data) {
        console.log('Uploading texture:', data);
        if (!data.name || !data.data || !data.width || !data.height) {
            console.error('Invalid texture data:', data);
            return;
        }
        console.log(data);
        if (this.emscriptenModule.ccall) {
            try {
                this.emscriptenModule.ccall('uploadTexture', null, ['string', 'string', 'number', 'number'], [
                    data.name,
                    data.data,
                    data.width,
                    data.height
                ]);
            }
            catch (err) {
                console.error('ccall uploadTexture failed:', err);
            }
        }
        else {
            console.error('Emscripten ccall not available');
        }
    }
    /**
     * Generate
     */
    generate() {
        if (!this.container) {
            console.error('container not found in DOM');
            return;
        }
        const data = this.getCurrentData();
        const dataObj = {
            name: data.name,
            shape: data.shape,
            size: data.size,
            color: data.color,
            colorRgb: data.color,
            texture: data.texture || data.texture.name,
            position: Number(data.position),
            lightning: data.lightning,
            hasSunLight: this.generatorConfig.setLightning(data.lightning),
            effects: data.effects,
            effectType: this.generatorConfig.setEffects(data.effects),
            rotationDir: data.rotationDir,
            rotationSpeedItself: data.rotationSpeedItself,
            rotationSpeedCenter: data.rotationSpeedCenter
        };
        const dataStr = JSON.stringify(dataObj);
        if (this.emscriptenModule._malloc &&
            this.emscriptenModule.stringToUTF8 &&
            this.emscriptenModule.lengthBytesUTF8) {
            try {
                const byteLength = this.emscriptenModule.lengthBytesUTF8(dataStr) + 1;
                const ptr = this.emscriptenModule._malloc(byteLength);
                if (ptr === 0) {
                    console.error('Failed to allocate memory');
                }
                else {
                    this.emscriptenModule.stringToUTF8(dataStr, ptr, byteLength);
                    this.emscriptenModule._generatePlanetParser(ptr);
                    this.emscriptenModule._free(ptr);
                    this.container.style.display = 'none';
                    return;
                }
            }
            catch (err) {
                console.error('Manual allocation failed:', err);
            }
        }
        if (this.emscriptenModule.ccall) {
            console.log('Using ccall');
            try {
                this.emscriptenModule.ccall('generatePlanetParser', null, ['string'], [dataStr]);
                this.container.style.display = 'none';
                return;
            }
            catch (err) {
                console.error('ccall failed:', err);
            }
        }
    }
    /**
     * Cancel Generation
     */
    cancelGeneration() {
        if (this.container) {
            this.container.style.display = 'none';
        }
        const controlList = document.querySelector('.controls--container');
        if (controlList) {
            controlList.style.display = 'flex';
        }
        if (this.emscriptenModule._cleanupPreview) {
            this.emscriptenModule._cleanupPreview();
        }
        else if (this.emscriptenModule.ccall) {
            this.emscriptenModule.ccall('cleanupPreview', null, [], []);
        }
    }
    /**
     * Setup Event Listeners
     */
    setupEventListeners() {
        if (!this.container) {
            console.error('setupEventListeners - modal not found in DOM');
            return;
        }
        /* Upload Texture */
        const texInput = this.container.querySelector('#planet-texture');
        texInput.addEventListener('change', (e) => {
            const file = e.target.files?.[0];
            if (file) {
                const reader = new FileReader();
                reader.onload = (event) => {
                    const result = event.target?.result;
                    const img = new Image();
                    img.onload = () => {
                        const base64Data = result.split(',')[1];
                        window.currentTexData = {
                            name: file.name.replace(/\.(jpg|jpeg|png|gif|bmp)$/i, ""),
                            path: file.name.replace(/\.(jpg|jpeg|png|gif|bmp)$/i, ""),
                            data: base64Data,
                            width: img.width,
                            height: img.height
                        };
                        console.log('currentTexData set:', window.currentTexData);
                        console.log('Data length:', base64Data.length);
                        this.uploadTexture(window.currentTexData);
                        debouncedUpdate();
                    };
                    img.onerror = () => {
                        console.error('Failed to load image for dimension detection');
                    };
                    img.src = result;
                };
                reader.onerror = () => {
                    console.error('FileReader error');
                };
                reader.readAsDataURL(file);
            }
        });
        /* Clear Texture */
        const clearTexBtn = this.container.querySelector('#btn-clear-tex');
        if (clearTexBtn) {
            clearTexBtn.addEventListener('click', () => {
                this.clearCurrentTexture();
            });
        }
        const updatePreview = () => {
            const data = this.getCurrentData();
            const dataStr = JSON.stringify(data);
            if (typeof window['updatePreviewPlanet'] === 'function') {
                window['updatePreviewPlanet'](dataStr);
                return;
            }
            if (this.emscriptenModule && this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall('updatePreviewPlanet', 'null', ['string'], [dataStr]);
                const texData = window.currentTexData;
                console.log('Texture data available for preview:', !!texData);
                return;
            }
            if (this.emscriptenModule && this.emscriptenModule._updatePreviewPlanet) {
                const lengthBytes = this.emscriptenModule.lengthBytesUTF8(dataStr) + 1;
                const ptr = this.emscriptenModule._malloc(lengthBytes);
                this.emscriptenModule.stringToUTF8(dataStr, ptr, lengthBytes);
                this.emscriptenModule._updatePreviewPlanet(ptr);
                this.emscriptenModule._free(ptr);
                return;
            }
        };
        let updateTimeout;
        const debouncedUpdate = () => {
            clearTimeout(updateTimeout);
            updateTimeout = setTimeout(updatePreview, 100);
        };
        if (this.generatorConfig) {
            this.generatorConfig.setupFormElementListeners(this.container, debouncedUpdate);
            this.generatorConfig.setupButtonListeners(this.container, () => this.generate(), () => this.cancelGeneration());
        }
    }
    onGenerate(cb) {
        this.onGenerateClick = cb;
    }
    onCancel(cb) {
        this.onCancelClick = cb;
    }
    /**
     * Reset Form Values
     */
    resetFormValues() {
        if (!this.container)
            return;
        this.generatorConfig.form.forEach(config => {
            const el = this.container.querySelector(`#${config.id}`);
            if (!el)
                return;
            let defaultVal;
            if (typeof config.defaultValue === 'function') {
                defaultVal = config.defaultValue();
            }
            else {
                defaultVal = config.defaultValue;
            }
            switch (config.type) {
                case 'text':
                case 'color':
                    el.value = defaultVal;
                    break;
                case 'select':
                    const select = el;
                    for (let i = 0; i < select.options.length; i++) {
                        if (select.options[i].value === defaultVal) {
                            select.selectedIndex = i;
                            break;
                        }
                    }
                    break;
                case 'range':
                    const range = el;
                    range.value = defaultVal.toString();
                    const valEl = this.container.querySelector(`#${config.id}-value`);
                    if (valEl) {
                        const precision = config.id.includes('rotation') ? 3 : 2;
                        valEl.textContent = Number(range.value).toFixed(precision);
                    }
                    break;
            }
        });
    }
    /**
     * Get Current Data
     */
    getCurrentData() {
        if (!this.container || !this.generatorConfig) {
            console.log('no container or config!');
            return {};
        }
        const data = {};
        this.generatorConfig.form.forEach(config => {
            const el = this.container.querySelector(`#${config.id}`);
            if (!el) {
                console.warn(`Element #${config.id} not found`);
                return;
            }
            const val = this.generatorConfig?.getElementValue(el, config.type);
            const map = this.generatorConfig?.fieldMap[config.id];
            if (map) {
                data[map.outputKey] = map.transform(val);
            }
            else {
                const key = this.generatorConfig?.idToCamelCase(config.id);
                data[key] = val;
            }
        });
        const texData = window.currentTexData;
        if (texData) {
            data.texture = texData.name || texData.path;
            console.log('Setting texture in data:', data.texture);
        }
        data.hasSunLight = data.lightning === "Sun Light";
        return data;
    }
    /**
     * Show Generator
     */
    showGenerator() {
        this.container.style.display = 'block';
        this.resetFormValues();
    }
    /**
     * Hide Generator
     */
    hideGenerator() {
        this.container.style.display = 'block';
        console.log("syysysy");
        this.resetFormValues();
    }
    /**
     * Setup Callbacks
     */
    setupCallbacks() {
        this.onGenerate((data) => {
            const dataStr = JSON.stringify(data);
            if (this.emscriptenModule._generatePlanetParser) {
                this.emscriptenModule._generatePlanetParser(dataStr);
            }
            else if (this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall('generatePlanetParser', null, ['string'], [dataStr]);
            }
            else {
                console.error('No valid Emscripten function found');
            }
        });
        this.onCancel(() => {
            console.log("TSTSTTSTST");
            if (this.emscriptenModule._cleanupPreview) {
                this.emscriptenModule._cleanupPreview();
                this.emscriptenModule._hideGenerator();
            }
            else if (this.emscriptenModule.ccall) {
                this.emscriptenModule.ccall('cleanupPreview', null, [], []);
                this.emscriptenModule.ccall('hideGenerator', null, [], []);
            }
            else if (this.emscriptenModule._hideGenerator) {
                console.log('hideeeeeeeeee');
                this.emscriptenModule._hideGenerator();
            }
        });
    }
    /**
     * Clear Texture
     */
    clearCurrentTexture() {
        const texInput = this.container?.querySelector('#planet-texture');
        if (texInput) {
            texInput.value = '';
        }
        if (window.currentTexData) {
            delete window.currentTexData;
        }
        if (this.emscriptenModule._clearCurrentTexture) {
            this.emscriptenModule._clearCurrentTexture();
        }
        else if (this.emscriptenModule.ccall) {
            this.emscriptenModule.ccall('clearCurrentTexture', null, [], []);
        }
        const texDisplay = this.container?.querySelector('#texture-display');
        if (texDisplay) {
            texDisplay.innerHTML = '';
        }
    }
}
