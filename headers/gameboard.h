#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include "blockshape.h"
#include "shapewidget.h"

class QGridLayout;
class QLabel;
class QDragEnterEvent;
class QDropEvent;
class QDragMoveEvent;
class QTimer;

class GameBoard : public QWidget {
    Q_OBJECT
public:
    explicit GameBoard(QWidget *parent = nullptr);
    void placeShape(const BlockShape &shape, int row, int col);
    bool canPlaceShape(const BlockShape &shape, int row, int col) const;
    bool canPlaceAnyShape(const BlockShape shapes[], ShapeWidget *shapeWidgets[]) const;
    bool canPlaceShapeOnTheBoard(const BlockShape &shape) const;
    void clearHighlight();
    void clearBoard();

signals:
    void dropReceived(int shapeId, const QPoint &pos);
    void scoreUpdated(int points);
    void linesCleared();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private slots:
    void animateClearing();
    void finishClearing();

private:
    void checkAndClearLines();

    QLabel *cells[12][12];
    QVector<QVector<bool>> grid;
    QGridLayout *gridLayout;
    QTimer *animationTimer;
    QVector<QPoint> cellsToClear;
    int animationStep;
};

#endif // GAMEBOARD_H

