#include "Board.h"
#include "res.h"

#include <functional>

using namespace oxygine;

// Константы для позиционирования доски
static constexpr int BOARD_OFFSET_X = 291;
static constexpr int BOARD_OFFSET_Y = 75;
static constexpr int CELL_SIZE = 71;

namespace {
    std::string getBaseNameForType(const PieceType type) {
        switch (type) {
            case PieceType::Pawn:   return "pawn";
            case PieceType::Rook:   return "rook";
            case PieceType::Knight: return "knight";
            case PieceType::Bishop: return "bishop";
            case PieceType::Queen:  return "queen";
            case PieceType::King:   return "king";
            default: return "";
        }
    }
}

void Board::init() {
    _gameOver = false;
    if (_messageSprite) {
        _messageSprite->detach();
        _messageSprite = nullptr;
    }
    // очищаем массив
    for (int r = 0; r < 8; ++r)
        for (int c =  0; c < 8; ++c)
            _pieces[r][c] = nullptr;

    // пешки
    for (int c = 0; c < 8; ++c) {
        CreatePiece(PieceType::Pawn, PieceColor::White, 6, c); // белые пешки на 6-й ряду (снизу)
        CreatePiece(PieceType::Pawn, PieceColor::Black, 1, c); // чёрные пешки на 1-м ряду
    }

    // ладьи
    CreatePiece(PieceType::Rook, PieceColor::White, 7, 0);
    CreatePiece(PieceType::Rook, PieceColor::White, 7, 7);
    CreatePiece(PieceType::Rook, PieceColor::Black, 0, 0);
    CreatePiece(PieceType::Rook, PieceColor::Black, 0, 7);

    // кони
    CreatePiece(PieceType::Knight, PieceColor::White, 7, 1);
    CreatePiece(PieceType::Knight, PieceColor::White, 7, 6);
    CreatePiece(PieceType::Knight, PieceColor::Black, 0, 1);
    CreatePiece(PieceType::Knight, PieceColor::Black, 0, 6);

    // слоны
    CreatePiece(PieceType::Bishop, PieceColor::White, 7, 2);
    CreatePiece(PieceType::Bishop, PieceColor::White, 7, 5);
    CreatePiece(PieceType::Bishop, PieceColor::Black, 0, 2);
    CreatePiece(PieceType::Bishop, PieceColor::Black, 0, 5);

    // ферзи
    CreatePiece(PieceType::Queen, PieceColor::White, 7, 3);
    CreatePiece(PieceType::Queen, PieceColor::Black, 0, 3);

    // короли
    CreatePiece(PieceType::King, PieceColor::White, 7, 4);
    CreatePiece(PieceType::King, PieceColor::Black, 0, 4);

    // создаем невидимые спрайты клеток и после создания фигур отключаем у них touch
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (_pieces[r][c])
                _pieces[r][c]->setTouchEnabled(false);

    // клетки для обработки кликов
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c)
        {
            spActor cell = new Actor();
            cell->setSize(CELL_SIZE, CELL_SIZE);
            cell->setPosition(BOARD_OFFSET_X + c * CELL_SIZE, BOARD_OFFSET_Y + r * CELL_SIZE);
            cell->setTouchEnabled(true);
            cell->addEventListener(TouchEvent::CLICK, [this, r, c](Event* ev) {
                OnCellClick(ev, r, c);
            });
            addChild(cell);
            _cells[r][c] = cell;
        }
    }
}

void Board::reset() {
    removeChildren();

    // сбрасываем состояние игры
    _selectedPiece = nullptr;
    _selectedRow = -1;
    _selectedCol = -1;
    _whiteTurn = true;

    _castlingRookFromRow = -1;
    _castlingRookFromCol = -1;
    _castlingRookToRow = -1;
    _castlingRookToCol = -1;

    // заново создаём фигуры
    init();
}

bool Board::IsSquareAttacked(int row, int col, PieceColor attackerColor) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            spPiece piece = _pieces[r][c];
            if (!piece || piece->GetPieceColor() != attackerColor)
                continue;

            if (CanAttack(r, c, row, col))
                return true;
        }
    }
    return false;
}

bool Board::IsKingSafe(const PieceColor color) const {
    // ищем короля
    int kingRow = -1, kingCol = -1;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            spPiece piece = _pieces[r][c];
            if (piece && piece->GetPieceType() == PieceType::King && piece->GetPieceColor() == color)
            {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
        if (kingRow != -1) break;
    }
    if (kingRow == -1) return false; // король не найден (тк не должно быть)

    PieceColor opponent = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    return !IsSquareAttacked(kingRow, kingCol, opponent);
}

bool Board::HasAnyValidMove(const PieceColor color) {
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            spPiece piece = _pieces[fromRow][fromCol];
            if (!piece || piece->GetPieceColor() != color)
                continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    if (fromRow == toRow && fromCol == toCol)
                        continue;

                    if (!IsValidMove(fromRow, fromCol, toRow, toCol))
                        continue;

                    if (IsMoveSafe(fromRow, fromCol, toRow, toCol))
                        return true;
                }
            }
        }
    }
    return false;
}

bool Board::IsMoveSafe(const int fromRow, const int fromCol, const int toRow, const int toCol) {
    const spPiece moving = _pieces[fromRow][fromCol];
    if (!moving) return false;
    const PieceColor color = moving->GetPieceColor();

    const spPiece target = _pieces[toRow][toCol];

    // Временное перемещение
    _pieces[fromRow][fromCol] = nullptr;
    _pieces[toRow][toCol] = moving;

    const bool safe = IsKingSafe(color);

    // Откат
    _pieces[fromRow][fromCol] = moving;
    _pieces[toRow][toCol] = target;

    return safe;
}

void Board::OnCellClick(Event* ev, const int row, const int col) {
    oxygine::logs::messageln("Cell clicked: (%d, %d)", row, col);
    if (_gameOver) return;

    spPiece piece = _pieces[row][col];

    if (!_selectedPiece) {
        if (piece) {
            const bool whiteTurnCondition = (_whiteTurn && piece->GetPieceColor() == PieceColor::White);
            const bool blackTurnCondition = (!_whiteTurn && piece->GetPieceColor() == PieceColor::Black);
            if (whiteTurnCondition || blackTurnCondition) {
                SelectPiece(row, col);
            } else {
                // Не та фигура
            }
        }
        // если нет фигуры или не та - ничего не делаем
    } else {
        // второй клик - ход
        MoveSelectedPiece(row, col);
    }
}

void Board::CreatePiece(const PieceType type, const PieceColor color, const int row, const int col) {
    std::string animName = getBaseNameForType(type);
    animName += (color == PieceColor::White) ? "_wh" : "_bl";

    const ResAnim* anim = res::ui.getResAnim(animName.c_str());
    if (!anim) {
        OX_ASSERT(!"ResAnim not found");
        return;
    }

    const spPiece piece = new Piece(type, color, row, col);
    piece->setResAnim(anim);
    piece->setAnchor(0.5f, 0.5f); // центр фигуры будет в центре клетки

    // вычисляем позицию в пикселях (центр клетки)
    const float x = BOARD_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
    const float y = BOARD_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;
    piece->setPosition(x, y);
    piece->setTouchEnabled(false);

    // добавляем на доску
    addChild(piece);

    // сохраняем в массив
    _pieces[row][col] = piece;
}

void Board::SelectPiece(int row, int col) {
    _selectedPiece = _pieces[row][col];
    if (!_selectedPiece) {
        return;
    }
    _selectedRow = row;
    _selectedCol = col;

    // подсветка
    std::string selectedAnim = GetSelectedAnimName(
        _selectedPiece->GetPieceType());

    ResAnim* anim = res::ui.getResAnim(selectedAnim.c_str());
    if (anim) {
        _selectedPiece->setResAnim(anim);
    }
}

void Board::CancelSelection() {
    if (_selectedPiece) {
        // возвращаем обычную текстуру
        const std::string normalAnim = GetNormalAnimName(_selectedPiece->GetPieceType(), _selectedPiece->GetPieceColor());
        ResAnim* anim = res::ui.getResAnim(normalAnim.c_str());
        if (anim) _selectedPiece->setResAnim(anim);
        _selectedPiece = nullptr;
        _selectedRow = -1;
        _selectedCol = -1;
    }
}

void Board::MoveSelectedPiece(const int row, const int col) {
    if (!_selectedPiece) return;

    if (!IsValidMove(_selectedRow, _selectedCol, row, col)) {
        // неверный ход по правилам
        CancelSelection();
        return;
    }

    if (!IsMoveSafe(_selectedRow, _selectedCol, row, col)) {
        printf("Move would leave king in check!\n");
        CancelSelection();
        return;
    }

    // возвращаем обычную текстуру (снимаем подсветук)
    std::string normalAnim = GetNormalAnimName(
        _selectedPiece->GetPieceType(),
        _selectedPiece->GetPieceColor()
    );
    ResAnim* anim = res::ui.getResAnim(normalAnim.c_str());
    if (anim)
        _selectedPiece->setResAnim(anim);

    _pieces[_selectedRow][_selectedCol] = nullptr;

    // если на целевой клетке есть фигура противника - удаляем
    spPiece targetPiece = _pieces[row][col];
    if (targetPiece) {
        targetPiece->detach();
    }

    // новые координаты
    float x = BOARD_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
    float y = BOARD_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;

    _selectedPiece->MoveTo(x, y, true);
    _selectedPiece->SetBoardPos(row, col);

    // обновляем массив
    _pieces[row][col] = _selectedPiece;

    // проверка на превращение пешки
    if (_selectedPiece->GetPieceType() == PieceType::Pawn) {
        if ((_selectedPiece->GetPieceColor() == PieceColor::White && row == 0) ||
            (_selectedPiece->GetPieceColor() == PieceColor::Black && row == 7)) {
            PromotePawn(row, col);
            _selectedPiece = nullptr;
            _selectedRow = -1;
            _selectedCol = -1;
        }
    }

    // если была рокировка -перемещаем ладью
    if (_castlingRookFromRow != -1) {
        spPiece rook = _pieces[_castlingRookFromRow][_castlingRookFromCol];
        if (rook) {
            // удаляем ладью со старой позиции
            _pieces[_castlingRookFromRow][_castlingRookFromCol] = nullptr;

            float x = BOARD_OFFSET_X + _castlingRookToCol * CELL_SIZE + CELL_SIZE / 2;
            float y = BOARD_OFFSET_Y + _castlingRookToRow * CELL_SIZE + CELL_SIZE / 2;

            rook->MoveTo(x, y, true);
            rook->SetBoardPos(_castlingRookToRow, _castlingRookToCol);
            rook->SetMoved(); // помечаем, что ладья двигалась

            _pieces[_castlingRookToRow][_castlingRookToCol] = rook;
        }
        // сбрасываем информацию о рокировке
        _castlingRookFromRow = -1;
    }

    _selectedPiece->SetMoved(); // король двигался

    // снимаем выбор
    _selectedPiece = nullptr;
    _selectedRow = -1;
    _selectedCol = -1;

    // меняем очередь хода
    _whiteTurn = !_whiteTurn;

    // проверка состояние игры
    const PieceColor currentPlayer = _whiteTurn ? PieceColor::White : PieceColor::Black;
    const bool inCheck = !IsKingSafe(currentPlayer);

    if (inCheck) {
        printf("Check! %s king is under attack.\n", currentPlayer == PieceColor::White ? "White" : "Black");
    }

    if (!HasAnyValidMove(currentPlayer))
    {
        if (inCheck)
        {
            // мат
            std::string winner = (currentPlayer == PieceColor::White) ? "black_wins" : "white_wins";
            ShowEndGameMessage(winner);
            printf("Checkmate! %s wins.\n", currentPlayer == PieceColor::White ? "Black" : "White");
        }
        else
        {
            // пат
            ShowEndGameMessage("draw");
            printf("Stalemate!\n");
        }
        _gameOver = true;
    }
}

bool Board::PointToCell(const Vector2& point, int& row, int& col) const {
    // проверяем, находится ли точка в пределах доски
    if (point.x < BOARD_OFFSET_X || point.x > BOARD_OFFSET_X + 8 * CELL_SIZE ||
        point.y < BOARD_OFFSET_Y || point.y > BOARD_OFFSET_Y + 8 * CELL_SIZE) {
        return false;
        }

    // индексы клетки
    col = (point.x - BOARD_OFFSET_X) / CELL_SIZE;
    row = (point.y - BOARD_OFFSET_Y) / CELL_SIZE;

    // коррекция, если точка попала на границу
    if (col < 0) col = 0;
    if (col > 7) col = 7;
    if (row < 0) row = 0;
    if (row > 7) row = 7;

    return true;
}

std::string Board::GetSelectedAnimName(const PieceType type) const {
    std::string base = getBaseNameForType(type);
    base += "_sel";
    return base;
}

std::string Board::GetNormalAnimName(const PieceType type, const PieceColor color) const {
    std::string base = getBaseNameForType(type);
    base += (color == PieceColor::White) ? "_wh" : "_bl";
    return base;
}

bool Board::IsPathClear(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    int rowStep = 0, colStep = 0;

    if (fromRow < toRow) rowStep = 1;
    else if (fromRow > toRow) rowStep = -1;

    if (fromCol < toCol) colStep = 1;
    else if (fromCol > toCol) colStep = -1;

    int row = fromRow + rowStep;
    int col = fromCol + colStep;

    while (row != toRow || col != toCol) {
        if (_pieces[row][col])
            return false;

        row += rowStep;
        col += colStep;
    }

    return true;
}

bool Board::IsValidMove(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    // проверка, что клетки в пределах доски
    if (fromRow < 0 || fromRow >= 8 || fromCol < 0 || fromCol >= 8 ||
        toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8)
        return false;

    // нельзя остаться на месте
    if (fromRow == toRow && fromCol == toCol)
        return false;

    spPiece piece = _pieces[fromRow][fromCol];
    if (!piece) return false; // нет фигуры

    spPiece target = _pieces[toRow][toCol];

    // Нельзя съесть свою фигуру
    if (target && target->GetPieceColor() == piece->GetPieceColor()) {
        return false;
    }

    return CanAttack(fromRow, fromCol, toRow, toCol);
}

bool Board::IsValidPawnMove(const int fromRow, const int fromCol, const int toRow,
    const int toCol, const PieceColor color) const {
    const int direction = (color == PieceColor::White) ? -1 : 1;
    const int startRow = (color == PieceColor::White) ? 6 : 1;

    // движение на одну клетку
    if (fromCol == toCol && toRow == fromRow + direction) {
        return IsPathClear(fromRow, fromCol, toRow, toCol) && !_pieces[toRow][toCol];
    }

    // движение на две клетки (только с начальной позиции)
    if (fromCol == toCol && fromRow == startRow && toRow == fromRow + 2 * direction) {
        return IsPathClear(fromRow, fromCol, toRow, toCol) && !_pieces[toRow][toCol];
    }

    // взятие по диагонали
    if (abs(toCol - fromCol) == 1 && toRow == fromRow + direction) {
        spPiece target = _pieces[toRow][toCol];
        return target && target->GetPieceColor() != color;
    }

    return false;
}

bool Board::IsValidRookMove( const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    if (fromRow != toRow && fromCol != toCol)
        return false;

    return IsPathClear(fromRow, fromCol, toRow, toCol);
}

bool Board::IsValidKnightMove(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);

    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool Board::IsValidBishopMove(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);

    // слон ходит по диагонали: разница по строкам и столбцам должна быть одинаковой
    if (rowDiff != colDiff)
        return false;

    return IsPathClear(fromRow, fromCol, toRow, toCol);
}

bool Board::IsValidQueenMove(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    // ферзь ходит как ладья или слон
    return (IsValidBishopMove(fromRow, fromCol, toRow, toCol) ||
        IsValidRookMove(fromRow, fromCol, toRow, toCol));
}

bool Board::IsValidKingMove(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    const int rowDiff = abs(toRow - fromRow);
    const int colDiff = abs(toCol - fromCol);

    if (rowDiff <= 1 && colDiff <= 1)
        return true;

    // Рокировка
    if (rowDiff == 0 && colDiff == 2)
    {
        spPiece king = _pieces[fromRow][fromCol];
        if (!king) return false;

        int rookFromCol, rookToCol;
        if (CanCastle(fromRow, fromCol, toCol, king->GetPieceColor(), rookFromCol, rookToCol)) {
            // сохраняем информацию для последующего перемещения ладьи
            _castlingRookFromRow = fromRow;
            _castlingRookFromCol = rookFromCol;
            _castlingRookToRow   = fromRow;
            _castlingRookToCol   = rookToCol;
            return true;
        } else {
            _castlingRookFromRow = -1;
            return false;
        }
    }
    return false;
}

bool Board::CanAttack(const int fromRow, const int fromCol, const int toRow, const int toCol) const {
    spPiece piece = _pieces[fromRow][fromCol];
    if (!piece) return false;

    switch (piece->GetPieceType())
    {
        case PieceType::Pawn:
            return IsValidPawnMove(fromRow, fromCol, toRow, toCol, piece->GetPieceColor());
        case PieceType::Rook:
            return IsValidRookMove(fromRow, fromCol, toRow, toCol);
        case PieceType::Knight:
            return IsValidKnightMove(fromRow, fromCol, toRow, toCol);
        case PieceType::Bishop:
            return IsValidBishopMove(fromRow, fromCol, toRow, toCol);
        case PieceType::Queen:
            return IsValidQueenMove(fromRow, fromCol, toRow, toCol);
        case PieceType::King:
            return IsValidKingMove(fromRow, fromCol, toRow, toCol);
        default:
            return false;
    }
}

bool Board::CanCastle(const int kingRow, const int kingCol, const int targetCol, const PieceColor color,
                      int& rookFromCol, int& rookToCol) const {
    spPiece king = _pieces[kingRow][kingCol];
    if (!king || king->HasMoved()) return false;

    // король не должен быть под шахом
    PieceColor opponent = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    if (IsSquareAttacked(kingRow, kingCol, opponent)) return false;

    // определяем сторону
    bool kingside = (targetCol > kingCol);
    rookFromCol = kingside ? 7 : 0;
    rookToCol = kingside ? kingCol + 1 : kingCol - 1; // для короткой f1/f8, для длинной d1/d8

    // ладья должна быть на месте и не двигаться
    spPiece rook = _pieces[kingRow][rookFromCol];
    if (!rook || rook->GetPieceType() != PieceType::Rook || rook->HasMoved())
        return false;

    // клетки между королем и ладьей должны быть пусты
    int step = kingside ? 1 : -1;
    for (int c = kingCol + step; c != rookFromCol; c += step)
        if (_pieces[kingRow][c]) return false;

    // поля, через которые проходит король (включая начальное и конечное), не должны быть атакованы
    const int startCheck = std::min(kingCol, targetCol);
    const int endCheck   = std::max(kingCol, targetCol);
    for (int c = startCheck; c <= endCheck; ++c)
        if (IsSquareAttacked(kingRow, c, opponent)) return false;

    return true;
}

void Board::PromotePawn(int row, int col) {
    spPiece pawn = _pieces[row][col];
    if (!pawn || pawn->GetPieceType() != PieceType::Pawn)
        return;

    PieceColor color = pawn->GetPieceColor();
    pawn->detach();
    _pieces[row][col] = nullptr;

    CreatePiece(PieceType::Queen, color, row, col);
}

void Board::ShowEndGameMessage(const std::string& animName)
{
    if (_messageSprite) {
        _messageSprite->detach();
        _messageSprite = nullptr;
    }
    _messageSprite = new Sprite();
    ResAnim* anim = res::ui.getResAnim(animName.c_str());
    if (!anim) {
        OX_ASSERT(!"Message anim not found");
        return;
    }
    _messageSprite->setResAnim(anim);
    _messageSprite->setAnchor(0.5f, 0.5f);

    float boardCenterX = BOARD_OFFSET_X + 4 * CELL_SIZE;
    float boardCenterY = BOARD_OFFSET_Y + 4 * CELL_SIZE;
    _messageSprite->setPosition(boardCenterX, boardCenterY);

    _messageSprite->setPriority(10);
    addChild(_messageSprite);
}