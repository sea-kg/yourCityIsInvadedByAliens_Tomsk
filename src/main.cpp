#include <iostream>
#include <vector>
#include <fstream>

#include "render_window.h"
#include "wsjcpp_core.h"


int main(int argc, char* args[]) {

    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        std::cerr << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cerr << "IMG_init has failed. Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return -1;
    }

    int nWindowWidth = 1280;
    int nWindowHeight = 720;

    RenderStateObjects stateObjects(nWindowWidth, nWindowHeight);

    RenderWindow window(
        "yourCityIsInvadedByAliens_Tomsk (v0.0.1)",
        stateObjects.windowWidth(),
        stateObjects.windowHeight()
    );

    // load map from json
    std::ifstream ifs("./res/data.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    
    nlohmann::json jsonBuildings = jf["buildings"];
    int nBuildingsSize = jsonBuildings.size();
    for (int i = 0; i < nBuildingsSize; i++) {
        nlohmann::json jsonBuilding = jsonBuildings[i];
        GameBuilding *pBuilding = new GameBuilding(jsonBuilding);
        stateObjects.addBuilding(pBuilding);
        window.addObject(new RenderBuilding(pBuilding));
    }

    SDL_Texture* grassTexture = window.loadTexture("res/gfx/ground_grass_1.png");
    
    // player
    window.addObject(new RenderPlayer0(CoordXY(nWindowWidth/2, nWindowHeight/2), 100));
    
    // text
    RenderAbsoluteTextBlock *pFpsText = new RenderAbsoluteTextBlock(CoordXY(10, 10), "FPS: ????", 1000);
    window.addObject(pFpsText);

    // textures
    // window.addObject(new RenderRectTexture(CoordXY(0, 0), grassTexture));
    // window.addObject(new RenderRectTexture(CoordXY(128, 0), grassTexture));
    // window.addObject(new RenderRectTexture(CoordXY(128, 128), grassTexture));
    // window.addObject(new RenderRectTexture(CoordXY(128, 256), grassTexture));
    // window.addObject(new RenderRectTexture(CoordXY(400, 400), grassTexture));
    // window.addObject(new RenderRectTexture(CoordXY(400, 250), grassTexture));

    // object
    // window.addObject(new RenderTriangle(
    //     CoordXY(320, 200),
    //     CoordXY(300, 240),
    //     CoordXY(340, 240)
    // ));
    window.sortObjectsByPositionZ();

    bool gameRunning = true;

    SDL_Event event;

    long nNumberOfFrames = 0;
    long nStartTime = WsjcppCore::getCurrentTimeInMilliseconds();
    long nElapsed = 0;
    stateObjects.init();
    while (gameRunning) {

        // Get our controls and events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:  stateObjects.incrementCoordLeftTopX(5); break;
                    case SDLK_RIGHT: stateObjects.incrementCoordLeftTopX(-5); break;
                    case SDLK_UP:    stateObjects.incrementCoordLeftTopY(5); break;
                    case SDLK_DOWN:  stateObjects.incrementCoordLeftTopY(-5); break;
                    case SDLK_w: stateObjects.incrementCoordLeftTopY(5); break;
                    case SDLK_s: stateObjects.incrementCoordLeftTopY(-5); break;
                    case SDLK_a:  stateObjects.incrementCoordLeftTopX(5); break;
                    case SDLK_d: stateObjects.incrementCoordLeftTopX(-5); break;
                }
                std::cout << "SDL_KEYDOWN" << std::endl;
            }
        }

        stateObjects.updateElapsedTime();
        window.clear();
        window.modifyObjects(stateObjects);
        window.drawObjects();

        // FPS
        nNumberOfFrames++;
        nElapsed = WsjcppCore::getCurrentTimeInMilliseconds() - nStartTime;
        if (nElapsed > 3000) {
            double nFPS = nNumberOfFrames;
            nFPS /= nElapsed;
            nFPS *= 1000;
            pFpsText->updateText("FPS: " + std::to_string(nFPS));
            std::cout << "FPS: " << nFPS << std::endl;
            nStartTime = WsjcppCore::getCurrentTimeInMilliseconds();
            nNumberOfFrames = 0;
        }
    }

    window.cleanUp();
    SDL_Quit();
    return 0;
}