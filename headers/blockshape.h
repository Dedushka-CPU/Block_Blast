#pragma once
#include <QVector>
#include <QString>
#include <QDataStream>

class BlockShape {
public:
    struct Cell {
        int row, col;
    };

    QVector<Cell> cells;
    QString color; 
    QString originalColor;
    int id;
    bool placed = false;

    BlockShape(const QVector<Cell> &c = {}, const QString &clr = "", int i = 0);
    void changeColor(bool change);
    void resetColor();
};

QDataStream &operator<<(QDataStream &out, const BlockShape::Cell &cell);
QDataStream &operator>>(QDataStream &in, BlockShape::Cell &cell);

