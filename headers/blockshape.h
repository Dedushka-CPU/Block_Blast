#pragma once

//вроде класс фигур
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
    int id;

    BlockShape(const QVector<Cell> &c = {}, const QString &clr = "", int i = 0);
};

QDataStream &operator<<(QDataStream &out, const BlockShape::Cell &cell);
QDataStream &operator>>(QDataStream &in, BlockShape::Cell &cell);


