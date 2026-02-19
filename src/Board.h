#pragma once
#include "Piece.h"
#include "oxygine-framework.h"

DECLARE_SMART(Board, spBoard);

class Board : public oxygine::Actor {
public:
    Board() = default;

    void init();
    void reset();

    bool IsSquareAttacked(int row, int col, PieceColor attackerColor) const;
    bool IsKingSafe(PieceColor color) const;
    bool HasAnyValidMove(PieceColor color);
    // проверка, безопасен ли ход для короля
    bool IsMoveSafe(int fromRow, int fromCol, int toRow, int toCol);

private:
    void OnCellClick(oxygine::Event* ev, int row, int col);
    void CreatePiece(PieceType type, PieceColor color, int row, int col);
    void SelectPiece(int row, int col);
    void CancelSelection();
    void MoveSelectedPiece(int row, int col);

    // преобразование координат клика в индекс клетки
    bool PointToCell(const oxygine::Vector2& point, int& row, int& col) const;

    std::string GetSelectedAnimName(PieceType type) const;
    std::string GetNormalAnimName(PieceType type, PieceColor color) const;
    // првоерка чистоты линии хода
    bool IsPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidPawnMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor color) const;
    bool IsValidRookMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidKnightMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidBishopMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidQueenMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool IsValidKingMove(int fromRow, int fromCol, int toRow, int toCol) const;

    bool CanAttack(int fromRow, int fromCol, int toRow, int toCol) const;
    bool CanCastle(int kingRow, int kingCol, int targetCol, PieceColor color,
                       int& rookFromCol, int& rookToCol) const;
    // превращение пешки в ферзя
    void PromotePawn(int row, int col);

    spPiece _pieces[8][8]; // двумерный массив фигур
    oxygine::spActor _cells[8][8]; // массив клеток

    // временные переменные для обработки хода
    spPiece _selectedPiece;
    int _selectedRow = -1;
    int _selectedCol = -1;
    bool _whiteTurn = true; // белые ходят первыми

    mutable int _castlingRookFromRow = -1;
    mutable int _castlingRookFromCol = -1;
    mutable int _castlingRookToRow = -1;
    mutable int _castlingRookToCol = -1;

    bool _gameOver = false;
    oxygine::spSprite _messageSprite;
    void ShowEndGameMessage(const std::string& animName);
};