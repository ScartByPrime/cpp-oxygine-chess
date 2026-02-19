#include "ox/oxygine.hpp"

#include "res.h"
#include "MainMenuScene.h"
#include "GameScene.h"

using namespace oxygine;

spMainMenuScene mainMenuScene;
spGameScene gameScene;

int mainloop()
{
    bool done = core::update();


    getStage()->update();

    if (core::beginRendering())
    {
        Color clearColor(32, 32, 32, 255);
        Rect viewport(Point(0, 0), core::getDisplaySize());
        getStage()->render(clearColor, viewport);
        core::swapDisplayBuffers();
    }

    return done ? 1 : 0;
}

// точка входа
void run()
{
    ObjectBase::__startTracingLeaks();

    core::init_desc desc;
    desc.title = "Oxygine Chess";
    desc.w = 1152;
    desc.h = 720;


    core::init(&desc);

    Stage::instance = new Stage();
    Point size = core::getDisplaySize();
    getStage()->setSize(size);

    DebugActor::show();

    res::load();

    mainMenuScene = new MainMenuScene();
    gameScene = new GameScene();

    // присваиваем статические указатели (для совместимости со старым кодом)
    MainMenuScene::instance = mainMenuScene;
    GameScene::instance = gameScene;

    getStage()->addChild(mainMenuScene->getView());

#ifdef EMSCRIPTEN
    return;
#endif

    // Главный цикл
    while (1)
    {
        int done = mainloop();
        if (done)
            break;
    }

    mainMenuScene = nullptr;
    gameScene = nullptr;
    res::free();

    core::release();

    ObjectBase::dumpCreatedObjects();
    ObjectBase::__stopTracingLeaks();
}

#ifdef OXYGINE_SDL
#include "SDL_main.h"
#include "SDL.h"

extern "C"
{
    void one(void* param) { mainloop(); }
    void oneEmsc() { mainloop(); }

    int main(int argc, char* argv[])
    {
        run();

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        // If parameter 2 is set to 1, refresh rate will be 60 fps, 2 - 30 fps, 3 - 15 fps.
        //SDL_iPhoneSetAnimationCallback(core::getWindow(), 1, one, nullptr);
#endif

#if EMSCRIPTEN
        emscripten_set_main_loop(oneEmsc, 0, 0);
#endif

        return 0;
    }
};
#endif