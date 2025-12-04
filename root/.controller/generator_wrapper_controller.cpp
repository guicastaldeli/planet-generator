#include "generator_wrapper_controller.h"
#include "../_data/data_parser.h"
#include "../.preset/preset_data.h"
#include "../.buffers/buffer_generator.h"
#include "../.preset/preset_loader.h"
#include "../.controller/buffer_controller.h"
#include "../.buffers/buffers.h"
#include <iostream>

GeneratorWrapperController* g_generatorWrapperController = nullptr;

GeneratorWrapperController::GeneratorWrapperController(
    PresetLoader* presetLoader,
    BufferController* bufferController
) :
    presetLoader(presetLoader),
    bufferController(bufferController) 
{
    if (!presetLoader || !bufferController) {
        std::cerr << "GeneratorWrapperController: Invalid pointers provided!" << std::endl;
    }
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

    /*
     * Show Generator
     */
    void showGenerator() {
        EM_ASM({
            (function() {
                const container = document.querySelector('#planet-creator-modal');
                if(container) {
                    container.style.display = 'block';
                } else {
                    console.error('Generator err');
                }
            })();
        });
        g_generatorWrapperController->
            bufferController->
            previewController->startGeneratorPreview();
    }

    /*
     * Start Generator Preview
     */
    void startGeneratorPreview() {
        if(g_generatorWrapperController && g_generatorWrapperController->bufferController) {
            g_generatorWrapperController->
                bufferController->
                previewController->startGeneratorPreview();
        }
    }

    /*
     * Update Preview
     */
    void updatePreviewPlanet(const char* data) {
        if(!g_generatorWrapperController || !data) {
            printf("ERR: cannot update preview!");
            return;
        }

        try {
            std::string dataStr(data);
            auto data = DataParser::Parser::parse(dataStr);

            PlanetBuffer planetBuffer;
            planetBuffer.isPreview = true;
            PlanetData previewPlanet;
            previewPlanet.id = -1;
            previewPlanet.name = 
                data.hasKey("name") ?
                data["name"].asString() :
                "Planet";

            if(data.hasKey("shape")) {
                std::string shape = data["shape"].asString();
                previewPlanet.shape = g_generatorWrapperController->
                    bufferController->
                    bufferGenerator->shapeToBufferType(shape);
            } else {
                previewPlanet.shape = BufferData::Type::SPHERE;
            }
            previewPlanet.size = 
                data.hasKey("size") ? 
                data["size"].asFloat() : 
                1.0f;
            previewPlanet.color =
                data.hasKey("color") ?
                data["color"].asString() :
                "#808080";

            if(data.hasKey("rotationDir")) {
                std::string rotation = data["rotationDir"].asString();
                previewPlanet.rotationDir = g_generatorWrapperController->
                    bufferController->
                    bufferGenerator->rotationToBufferType(rotation);
            } else {
                previewPlanet.rotationDir = RotationAxis::Y;
            }

            previewPlanet.rotationSpeedItself = 
                data.hasKey("rotationSpeedItself") ?
                data["rotationSpeedItself"].asFloat() :
                0.01f;
            previewPlanet.rotationSpeedCenter =
                data.hasKey("rotationSpeedCenter") ?
                data["rotationSpeedCenter"].asFloat() :
                0.01f;

            g_generatorWrapperController->
                bufferController->
                previewController->updatePreview(previewPlanet);
        } catch(const std::exception& err) {
            printf("Error updating preview planet: %s\n", err.what());
        }
    }

    /*
     * Generate Planet
     */
    void generatePlanetParser(const char* planetData) {
        g_generatorWrapperController->
            bufferController->
            previewController->exitPreview();

        printf("Received planet data: %s\n", planetData);
        if(!planetData || strlen(planetData) == 0) {
            printf("ERR: Empty planet data received\n");
            return;
        }
        if(!g_generatorWrapperController) {
            printf("ERR: Generator wrapper controller not initialized\n");
            return;
        }

        try {
            std::string str(planetData);
            printf("Parsing JSON string of length: %zu\n", str.length());
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
            auto newPlanetBuffers = g_generatorWrapperController->
                bufferController->
                bufferGenerator->generateFromPreset(preset);

            g_generatorWrapperController->bufferController->buffers->planetBuffers.clear();
            for(auto& planetBuffer : newPlanetBuffers) {
                float orbitRadius = planetBuffer.data.distanceFromCenter;
                float initialAngle = planetBuffer.data.orbitAngle.y;
                glm::vec3 planetPosition(
                    orbitRadius * cos(glm::radians(initialAngle)),
                    0.0f,
                    orbitRadius * sin(glm::radians(initialAngle))
                );
                planetBuffer.worldPos = planetPosition;
                g_generatorWrapperController->bufferController->buffers->createBufferForPlanet(planetBuffer);
                g_generatorWrapperController->bufferController->buffers->planetBuffers.push_back(std::move(planetBuffer));
            }

            printf("Generated planet: %s at position %d\n", newPlanet.name.c_str(), newPlanet.position);
        } catch(const std::exception& e) {
            printf("Error generating planet: %s\n", e.what());
        }
    }
}