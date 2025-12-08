#include "generator_wrapper_controller.h"
#include "../_data/data_parser.h"
#include "../.preset/preset_data.h"
#include "../.buffers/buffer_generator.h"
#include "../.preset/preset_loader.h"
#include "../.controller/buffer_controller.h"
#include "preview_controller.h"
#include "../.buffers/buffers.h"
#include "../_utils/color_converter.h"
#include <iostream>
#include <sstream>

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
                    if(window.generatorController) {
                        window.generatorController.showGenerator();
                    }
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
        if(!g_generatorWrapperController) {
            printf("ERROR: g_generatorWrapperController is null!\n");
            return;
        }
        if(!data) {
            printf("ERROR: data pointer is null!\n");
            return;
        }
        if(strlen(data) == 0) {
            printf("ERROR: data string is empty!\n");
            return;
        }

        try {
            std::string dataStr(data);
            auto planetData = DataParser::Parser::parse(dataStr);
            PlanetData previewPlanetData;

            g_generatorWrapperController->
                bufferController->
                    defaultData->init();
        
            g_generatorWrapperController->
                bufferController->
                    setDataToUpdate(previewPlanetData, planetData);
            
            printf("Calling updatePreview with planet: %s, shape: %d\n", 
                previewPlanetData.name.c_str(), previewPlanetData.shape);

            g_generatorWrapperController->
                bufferController->
                previewController->updatePreview(previewPlanetData);
        } catch(const std::exception& err) {
            printf("Error updating preview planet: %s\n", err.what());
        }
    }

    /*
     * Cleanup Preview
     */
    void cleanupPreview() {
        if(g_generatorWrapperController && g_generatorWrapperController->bufferController) {
            g_generatorWrapperController->
                bufferController->
                previewController->cleanupPreview();
        }
    }

    /*
     * Hide Generator
     */
    void hideGenerator() {
        EM_ASM({
            const container = document.querySelector('#planet-creator-modal');
            if(container) {
                container.style.display = 'none';
            }
        });
        cleanupPreview();
    }

    /*
     * Generate Planet
     */
    void generatePlanetParser(const char* planetData) {
        cleanupPreview();

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
            auto data = DataParser::Parser::parse(str);
            PlanetData newPlanet;

            g_generatorWrapperController->
                bufferController->
                    defaultData->init();
        
            g_generatorWrapperController->
                bufferController->
                    setDataToUpdate(newPlanet, data);

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
            g_generatorWrapperController->bufferController->currentPreset.planets.push_back(newPlanet);
            
            PlanetBuffer newPlanetBuffer;
            newPlanetBuffer.data = newPlanet;
            newPlanetBuffer.isPreview = false;

            float orbitRadius = newPlanet.distanceFromCenter;
            float initialAngle = newPlanet.orbitAngle.y;
            glm::vec3 planetPosition(
                orbitRadius * cos(glm::radians(initialAngle)),
                0.0f,
                orbitRadius * sin(glm::radians(initialAngle))
            );
            newPlanetBuffer.worldPos = planetPosition;

            g_generatorWrapperController->
                bufferController->
                buffers->createBufferForPlanet(newPlanetBuffer);
            
            g_generatorWrapperController->
                bufferController->
                buffers->
                planetBuffers.push_back(std::move(newPlanetBuffer));


            printf("Generated planet: %s at position %d\n", newPlanet.name.c_str(), newPlanet.position);
        } catch(const std::exception& e) {
            printf("Error generating planet: %s\n", e.what());
        }
    }

    /*
     * Get Default Data
     */
    const char* getDefaultData() {
        static std::string defaultDataStr;
        
        if(!g_generatorWrapperController || 
           !g_generatorWrapperController->bufferController ||
           !g_generatorWrapperController->bufferController->defaultData) {
            return "{}";
        }
        
        const auto& allData = g_generatorWrapperController->bufferController->defaultData->getAllData();
        if(allData.empty()) {
            return "{}";
        }
        
        const auto& defaultData = allData[0];
        std::stringstream str;
        str << "{";
        str << "\"name\":\"" << defaultData.name << "\",";
        str << "\"shape\":\"" << (defaultData.shape == BufferData::Type::SPHERE ? "SPHERE" : 
                                  defaultData.shape == BufferData::Type::CUBE ? "CUBE" : "TRIANGLE") << "\",";
        str << "\"size\":" << defaultData.size << ",";
        str << "\"color\":\"" << defaultData.color << "\",";
        str << "\"colorRgb\":{";
        str << "\"r\":" << defaultData.colorRgb.r << ",";
        str << "\"g\":" << defaultData.colorRgb.g << ",";
        str << "\"b\":" << defaultData.colorRgb.b;
        str << "},";
        str << "\"position\":" << defaultData.position << ",";
        str << "\"rotationDir\":\"" << (defaultData.rotationDir == RotationAxis::X ? "X" : 
                                       defaultData.rotationDir == RotationAxis::Y ? "Y" : "Z") << "\",";
        str << "\"rotationSpeedItself\":" << defaultData.rotationSpeedItself << ",";
        str << "\"rotationSpeedCenter\":" << defaultData.rotationSpeedCenter;
        str << "}";
        
        defaultDataStr = str.str();
        return defaultDataStr.c_str();
    }
}