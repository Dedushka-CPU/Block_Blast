#ifndef BLOCKSHAPE_H
#define BLOCKSHAPE_H

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

#endif // BLOCKSHAPE_H
