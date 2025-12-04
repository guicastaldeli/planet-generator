#include "generator_wrapper_controller.h"
#include "../_data/data_parser.h"
#include "../.preset/preset_data.h"
#include "../.buffers/buffer_generator.h"
#include "../.preset/preset_loader.h"
#include "../.controller/buffer_controller.h"

GeneratorWrapperController* g_generatorWrapperController = nullptr;

GeneratorWrapperController::GeneratorWrapperController(
    PresetLoader* presetLoader,
    BufferController* bufferController
) :
    presetLoader(presetLoader),
    bufferController(bufferController) 
{
    g_generatorWrapperController = this;
}
GeneratorWrapperController::~GeneratorWrapperController() {
    g_generatorWrapperController = nullptr;
}

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

    void generatePlanetParser(const char* planetData) {
        if(!g_generatorWrapperController) {
            printf("ERR.");
            return;
        }

        try {
            std::string str(planetData);
            auto data = DataParser::Parser::parse(str);

            PlanetData newPlanet;
            newPlanet.id = 9;
            if(data.hasKey("name")) {
                newPlanet.name = data["name"].asString();
            } else {
                newPlanet.name = "planet";
            }
            if(data.hasKey("shape")) {
                std::string shapeData = data["shape"].asString();
                newPlanet.shape = g_generatorWrapperController->
                    bufferController->
                    bufferGenerator->shapeToBufferType(shapeData);
            } else {
                newPlanet.shape = BufferData::Type::TRIANGLE;
            }
            if(data.hasKey("size")) {
                newPlanet.size = data["size"].asFloat();
            } else {
                newPlanet.size = 1.0f;
            }
            if(data.hasKey("color")) {
                newPlanet.color = data["color"].asString();
            } else {
                newPlanet.color = "#808080";
            }
            if(data.hasKey("position")) {
                newPlanet.position = data["position"].asInt();
            } else {
                newPlanet.position = 9;
            }
            if(data.hasKey("rotationDir")) {
                std::string rotationData = data["rotationDir"].asString();
                newPlanet.rotationDir = g_generatorWrapperController->
                    bufferController->
                    bufferGenerator->rotationToBufferType(rotationData);
            } else {
                newPlanet.rotationDir = RotationAxis::Y;
            }
            if(data.hasKey("rotationSpeedItself")) {
                newPlanet.rotationSpeedItself = data["rotationSpeedItself"].asFloat();
            } else {
                newPlanet.rotationSpeedItself = 0.01f;
            }
            if(data.hasKey("rotationSpeedCenter")) {
                newPlanet.rotationSpeedCenter = data["rotationSpeedCenter"].asFloat();
            } else {
                newPlanet.rotationSpeedCenter = 0.01f;
            }
            newPlanet.distanceFromCenter = 
                g_generatorWrapperController->
                bufferController->
                bufferGenerator->calculateDistanceFromPosition(newPlanet.position);

            auto& preset = g_generatorWrapperController->presetLoader->getCurrentPreset();

            bool positionOccupied = false;
            for(const auto& planet : preset.planets) {
                if(planet.position == newPlanet.position && planet.position != 0) {
                    positionOccupied = true;
                    break;
                }
            }
            
            if(positionOccupied) {
                newPlanet.position = g_generatorWrapperController->
                    bufferController->
                    bufferGenerator->findAvailablePosition(preset.planets);
                if(newPlanet.position == -1) {
                    g_generatorWrapperController->
                        bufferController->
                        bufferGenerator->replaceLastPlanet(preset.planets, newPlanet);
                }
            }

            preset.planets.push_back(newPlanet);
            if(g_generatorWrapperController->bufferController && 
                g_generatorWrapperController->bufferController->bufferGenerator
            ) {
                g_generatorWrapperController->
                bufferController->
                bufferGenerator->generatePlanet(newPlanet);
            }

            printf("Generated planet: %s at position %d\n", newPlanet.name.c_str(), newPlanet.position);
            
        } catch(const std::exception& e) {
            printf("Error generating planet: %s\n", e.what());
        }
    }
}