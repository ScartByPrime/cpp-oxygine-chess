#include "Piece.h"

using namespace oxygine;

Piece::Piece(const PieceType type, const PieceColor color, const int row, const int col)
    : _type(type), _color(color), _row(row), _col(col)
{
}

void Piece::MoveTo(const float x, const float y, const bool animate)
{
    if (animate)
    {
        addTween(Actor::TweenPosition(x, y), 300);
    }
    else
    {
        setPosition(x, y);
    }
}

void Piece::Capture()
{
    // анимация исчезновения
    spTween tween = addTween(Actor::TweenAlpha(0), 300);
    tween->addDoneCallback([this](Event*)
    {
        detach(); // удаляем сцены
    });
}