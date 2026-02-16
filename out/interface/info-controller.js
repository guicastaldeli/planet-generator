import { DocLoader } from "../../out/doc-loader.js";
export class InfoController {
    constructor(module) {
        this.container = null;
        this.emscriptenModule = module;
        this.loader = DocLoader.getInstance('./interface/_info.html');
        this.init();
    }
    async init() {
        await this.extractContainer();
        await this.append();
        this.setupEventListeners();
    }
    async append() {
        if (this.container && this.emscriptenModule) {
            const html = this.container.outerHTML;
            this.emscriptenModule.ccall('appendInfoToDOM', null, ['string'], [html]);
        }
    }
    /**
     * Display Info
     */
    display(name, info) {
        const domContainer = document.querySelector('.info--container');
        this.container = domContainer;
        domContainer.style.display = 'block';
        if (!domContainer) {
            console.error('Info container not found');
            return;
        }
        const actnsEl = domContainer.querySelector('#info--actns');
        if (actnsEl)
            actnsEl.style.display = 'block';
        const nameEl = domContainer.querySelector('#info--obj-name p');
        const infoEl = domContainer.querySelector('#info--obj-info p');
        const infoContainer = domContainer.querySelector('#info--obj-info');
        if (nameEl) {
            nameEl.textContent = name;
        }
        else {
            console.error('Name element not found');
        }
        if (infoEl && infoContainer) {
            if (info && info.trim() !== '') {
                infoEl.textContent = info;
                infoContainer.style.display = 'block';
            }
            else {
                infoEl.textContent = '';
                infoContainer.style.display = 'none';
            }
        }
        else {
            console.error('Info elements not found');
        }
    }
    /**
     * Extract Container
     */
    async extractContainer() {
        try {
            const doc = await this.loader.load();
            if (!doc)
                throw new Error('doc err');
            this.container = doc.querySelector('.info--container');
            return this.container;
        }
        catch (err) {
            console.error(err);
            return null;
        }
    }
    /**
     * Setup Event Listeners
     */
    setupEventListeners() {
        setTimeout(() => {
            const domContainer = document.querySelector('.info--container');
            this.container = domContainer;
            const closeButton = domContainer.querySelector('#info--close-actn');
            const deleteButton = domContainer.querySelector('#info--delete-actn');
            if (closeButton) {
                closeButton.addEventListener('click', (e) => {
                    e.stopPropagation();
                    e.stopImmediatePropagation();
                    if (this.emscriptenModule._closeMenu) {
                        this.emscriptenModule._closeMenu();
                    }
                    else if (this.emscriptenModule.ccall) {
                        this.emscriptenModule.ccall('closeMenu', null, [], []);
                    }
                });
            }
            if (deleteButton) {
                deleteButton.addEventListener('click', (e) => {
                    e.stopPropagation();
                    e.stopImmediatePropagation();
                    if (this.emscriptenModule._deletePlanet) {
                        this.emscriptenModule._deletePlanet();
                    }
                    else if (this.emscriptenModule.ccall) {
                        this.emscriptenModule.ccall('deletePlanet', null, [], []);
                    }
                });
            }
        }, 100);
    }
    setupCallbacks() {
        window.display = (name, info) => {
            this.display(name, info);
        };
    }
}
