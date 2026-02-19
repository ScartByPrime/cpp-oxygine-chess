#pragma once
#include "oxygine-framework.h"
#include "Board.h"

DECLARE_SMART(Game, spGame);

class Game : public oxygine::Actor {
public:
    Game() = default;

    void init();
    void reset();

private:
    spBoard _board; // указатель на доску
};