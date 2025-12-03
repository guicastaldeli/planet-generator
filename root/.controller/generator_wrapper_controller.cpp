#include "generator_wrapper_controller.h"

GeneratorWrapperController::GeneratorWrapperController() {}
GeneratorWrapperController::~GeneratorWrapperController() {}

extern "C" {
    void appendGeneratorToDOM(const char* html) {
        EM_ASM({
            const htmlString = UTF8ToString($0);

            const container = document.createElement('div');
            container.innerHTML = htmlString;

            const generatorContainer = container.querySelector('#planet-creator-modal');
            if(generatorContainer) {
                document.body.appendChild(generatorContainer);
                console.log('Generator container appended to DOM from C++');
            }
        }, html);
    }

    void showGenerator() {
        EM_ASM({
            const container = document.getElementById('planet-creator-modal');
            if(container) {
                container.style.display = 'block';

                const closeBtn = container.querySelector('.close');
                if(closeBtn) {
                    closeBtn.addEventListener('click', () => {
                        container.style.display = 'none';
                    });
                }

                window.addEventListener('click', (e) => {
                    if(e.target === container) {
                        container.style.display = 'none';
                    }
                });
            }
        });
    }

    void generate(const char* planetData) {
        EM_ASM({
            const data = JSON.parse(UTF8ToString($0));
            console.log('Generating planet:', data);


        }, planetData);
    }
}