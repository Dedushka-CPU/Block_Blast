#include "../headers/blockshape.h"
BlockShape::BlockShape(const QVector<Cell> &c, const QString &clr, int i)
    : cells(c), color(clr), originalColor(clr), id(i) {
}

void BlockShape::changeColor(bool change) {
    if (change) {
        color = originalColor;
    } else {
        color = "#808080";
    }
}

QDataStream &operator<<(QDataStream &out, const BlockShape::Cell &cell) {
    out << cell.row << cell.col;
    return out;
}

QDataStream &operator>>(QDataStream &in, BlockShape::Cell &cell) {
    in >> cell.row >> cell.col;
    return in;
}
