#include "GameScene.h"
#include "MainMenuScene.h"
#include "MyButton.h"
#include "res.h"

using namespace oxygine;

spGameScene GameScene::instance;

GameScene::GameScene()
{
    //create game actor
    _game = new Game;
    _game->init();
    _game->attachTo(_view);

    //create menu button
    spSprite btn = new MyButton;
    btn->setResAnim(res::ui.getResAnim("menu"));
    btn->setAnchor(0.5f, 0.5f);
    btn->attachTo(_view);

    //align it to top right
    btn->setX(_view->getWidth() - btn->getWidth() / 2);
    btn->setY(btn->getHeight() / 2);

    btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &GameScene::onEvent));
}
void GameScene::resetGame()
{
    if (_game)
        _game->reset();
}
void GameScene::onEvent(Event* ev)
{
    //clicked to menu button
    changeScene(MainMenuScene::instance);
}