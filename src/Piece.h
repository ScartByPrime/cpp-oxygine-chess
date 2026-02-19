#pragma once
#include "oxygine-framework.h"

DECLARE_SMART(Piece, spPiece);

enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };
enum class PieceColor { White, Black };

class Piece : public oxygine::Sprite
{
public:

    Piece(PieceType type, PieceColor color, int row, int col);
    virtual ~Piece() = default;

    PieceType GetPieceType() const { return _type; }
    PieceColor GetPieceColor() const { return _color; }
    int GetRow() const { return _row; }
    int GetCol() const { return _col; }
    void SetBoardPos(const int row, const int col) { _row = row; _col = col; }

    bool HasMoved() const { return _hasMoved; }
    void SetMoved() { _hasMoved = true; }

    void MoveTo(float x, float y, bool animate = true);
    void Capture();

private:
    PieceType _type;
    PieceColor _color;
    int _row;
    int _col;
    bool _hasMoved = false;
};