#include "Game.h"
#include "res.h"

using namespace oxygine;

void Game::init()
{
    setSize(getStage()->getSize());

    spSprite background = new Sprite();
    background->setResAnim(res::ui.getResAnim("background"));
    background->attachTo(this);

    _board = new Board();
    _board->attachTo(this);

    _board->init();
}

void Game::reset()
{
    if (_board)
        _board->reset();
}