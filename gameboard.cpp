#include "gameboard.h"
#include <QGridLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDragLeaveEvent>
#include <QTimer>

GameBoard::GameBoard(QWidget *parent) : QWidget(parent), animationTimer(nullptr), animationStep(0) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    grid = QVector<QVector<bool>>(12, QVector<bool>(12, false));

    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            cells[row][col] = new QLabel(this);
            cells[row][col]->setFixedSize(50, 50);
            cells[row][col]->setStyleSheet("background-color: #2a2a3d; border: 1px solid #3a3a4d;");
            gridLayout->addWidget(cells[row][col], row, col);
        }
    }

    setAcceptDrops(true);
    setFixedSize(12 * 52, 12 * 52);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &GameBoard::animateClearing);
}

void GameBoard::clearBoard() {
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            grid[row][col] = false;
            cells[row][col]->setStyleSheet("background-color: #2a2a3d; border: 1px solid #3a3a4d;");
        }
    }
}

void GameBoard::placeShape(const BlockShape &shape, int row, int col) {
    for (const auto &cell : shape.cells) {
        int r = row + cell.row;
        int c = col + cell.col;
        if (r >= 0 && r < 12 && c >= 0 && c < 12) {
            grid[r][c] = true;
            cells[r][c]->setStyleSheet(QString("background-color: %1; border: 1px solid #3a3a4d;").arg(shape.color));
        }
    }
    checkAndClearLines();
}

bool GameBoard::canPlaceShape(const BlockShape &shape, int row, int col) const {
    for (const auto &cell : shape.cells) {
        int r = row + cell.row;
        int c = col + cell.col;
        if (r < 0 || r >= 12 || c < 0 || c >= 12 || grid[r][c]) {
            return false;
        }
    }
    return true;
}

bool GameBoard::canPlaceAnyShape(const BlockShape shapes[], ShapeWidget *shapeWidgets[]) const {
    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i]) {
            const BlockShape &shape = shapes[i];
            for (int row = 0; row < 12; ++row) {
                for (int col = 0; col < 12; ++col) {
                    if (canPlaceShape(shape, row, col)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void GameBoard::clearHighlight() {
    for (int row = 0; row < 12; ++row) {
        for (int col = 0; col < 12; ++col) {
            if (!grid[row][col]) {
                cells[row][col]->setStyleSheet("background-color: #2a2a3d; border: 1px solid #3a3a4d;");
            }
        }
    }
}

void GameBoard::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-blockshape")) {
        event->acceptProposedAction();
    }
}

void GameBoard::dragMoveEvent(QDragMoveEvent *event) {
    clearHighlight();
    if (!event->mimeData()->hasFormat("application/x-blockshape")) {
        return;
    }

    QByteArray data = event->mimeData()->data("application/x-blockshape");
    QDataStream stream(&data, QIODevice::ReadOnly);
    int shapeId;
    QString color;
    QVector<BlockShape::Cell> cells;
    stream >> shapeId >> color;
    int cellCount;
    stream >> cellCount;
    cells.resize(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        stream >> cells[i];
    }

    BlockShape shape(cells, color, shapeId);
    int row = event->pos().y() / 50;
    int col = event->pos().x() / 50;
    bool canPlace = canPlaceShape(shape, row, col);
    QString highlightColor = canPlace ? color + "80" : "#ff000080";

    for (const auto &cell : shape.cells) {
        int r = row + cell.row;
        int c = col + cell.col;
        if (r >= 0 && r < 12 && c >= 0 && c < 12 && !grid[r][c]) {
            this->cells[r][c]->setStyleSheet(QString("background-color: %1; border: 1px solid #3a3a4d;").arg(highlightColor));
        }
    }
    event->acceptProposedAction();
}

void GameBoard::dragLeaveEvent(QDragLeaveEvent *) {
    clearHighlight();
}

void GameBoard::dropEvent(QDropEvent *event) {
    clearHighlight();
    QByteArray data = event->mimeData()->data("application/x-blockshape");
    QDataStream stream(&data, QIODevice::ReadOnly);
    int shapeId;
    QString color;
    QVector<BlockShape::Cell> cells;
    stream >> shapeId >> color;
    int cellCount;
    stream >> cellCount;
    cells.resize(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        stream >> cells[i];
    }

    emit dropReceived(shapeId, event->pos());
    event->acceptProposedAction();
}

void GameBoard::checkAndClearLines() {
    cellsToClear.clear();

    // Проверка горизонтальных линий
    for (int row = 0; row < 12; ++row) {
        bool full = true;
        for (int col = 0; col < 12; ++col) {
            if (!grid[row][col]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int col = 0; col < 12; ++col) {
                cellsToClear.append(QPoint(row, col));
            }
        }
    }

    // Проверка вертикальных линий
    for (int col = 0; col < 12; ++col) {
        bool full = true;
        for (int row = 0; row < 12; ++row) {
            if (!grid[row][col]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int row = 0; row < 12; ++row) {
                cellsToClear.append(QPoint(row, col));
            }
        }
    }

    // Проверка главных диагоналей (левый верх -> правый низ)
    bool full = true;
    for (int i = 0; i < 12; ++i) {
        if (!grid[i][i]) {
            full = false;
            break;
        }
    }
    if (full) {
        for (int i = 0; i < 12; ++i) {
            cellsToClear.append(QPoint(i, i));
        }
    }

    // Проверка побочных диагоналей (левый низ -> правый верх)
    full = true;
    for (int i = 0; i < 12; ++i) {
        if (!grid[11 - i][i]) {
            full = false;
            break;
        }
    }
    if (full) {
        for (int i = 0; i < 12; ++i) {
            cellsToClear.append(QPoint(11 - i, i));
        }
    }

    if (!cellsToClear.isEmpty()) {
        animationStep = 0;
        animationTimer->start(100);
    }
}

void GameBoard::animateClearing() {
    animationStep++;
    bool highlight = (animationStep % 2) == 1;

    for (const auto &cell : cellsToClear) {
        int row = cell.x();
        int col = cell.y();
        cells[row][col]->setStyleSheet(highlight ?
                                           "background-color: #ffffff; border: 1px solid #3a3a4d;" :
                                           "background-color: #2a2a3d; border: 1px solid #3a3a4d;");
    }

    if (animationStep >= 5) {
        animationTimer->stop();
        finishClearing();
    }
}

void GameBoard::finishClearing() {
    for (const auto &cell : cellsToClear) {
        int row = cell.x();
        int col = cell.y();
        grid[row][col] = false;
        cells[row][col]->setStyleSheet("background-color: #2a2a3d; border: 1px solid #3a3a4d;");
    }

    int linesCleared = cellsToClear.size() / 12;
    if (linesCleared > 0) {
        emit scoreUpdated(linesCleared * 50);
    }

    cellsToClear.clear();
}
