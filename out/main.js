"use strict";
const canvas = document.getElementById('ctx');
/**
 * Wait for Emscripten
 */
function waitForEmscripten() {
    return new Promise((resolve, reject) => {
        if (window.Module) {
            const originalCallback = window.Module.onRuntimeInitialized;
            window.Module.onRuntimeInitialized = () => {
                if (originalCallback)
                    originalCallback();
                console.log('Emscripten runtime initialized');
                resolve(window.Module);
            };
        }
        else {
            window.Module = {
                onRuntimeInitialized: () => {
                    console.log('Emscripten runtime initialized');
                    resolve(window.Module);
                }
            };
        }
        setTimeout(() => {
            reject(new Error('Emscripten module load timeout'));
        }, 10000);
    });
}
/**
 * Init App
 */
async function initApp() {
    try {
        console.log('Waiting for Emscripten...');
        resize();
        const module = await waitForEmscripten();
        console.log('Emscripten ready, initializing AppController...');
        const { AppController } = await import('../out/app-controller.js');
        const appController = new AppController(module);
        window.appController = appController;
        console.log('AppController initialized successfully');
    }
    catch (error) {
        console.error('Failed to initialize app:', error);
    }
}
/**
 * Disable Context Menu
 */
function disableContextMenu() {
    document.addEventListener('contextmenu', (e) => {
        e.preventDefault();
        return false;
    });
    canvas.addEventListener('contextmenu', (e) => {
        e.preventDefault();
        return false;
    });
}
/**
 * Resize Window
 */
function resize() {
    const width = window.innerWidth * window.devicePixelRatio;
    const height = window.innerHeight * window.devicePixelRatio;
    canvas.width = Math.max(1, width);
    canvas.height = Math.max(1, height);
    canvas.style.width = window.innerWidth + 'px';
    canvas.style.height = window.innerHeight + 'px';
}
async function run() {
    console.log('Starting application...');
    disableContextMenu();
    resize();
    window.addEventListener('resize', resize);
    await initApp();
    console.log('Application started');
}
document.addEventListener('DOMContentLoaded', run);
