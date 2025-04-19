#include "shapewidget.h"
#include <QPainter>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

ShapeWidget::ShapeWidget(const BlockShape &s, QWidget *parent) : QWidget(parent), shape(s) {
    setFixedSize(250, 250);
    setAcceptDrops(false);
}

const BlockShape &ShapeWidget::getShape() const {
    return shape;
}

void ShapeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int cellSize = 30;
    int offsetX = (width() - 4 * cellSize) / 2;
    int offsetY = (height() - 4 * cellSize) / 2;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(shape.color));
    for (const auto &cell : shape.cells) {
        painter.drawRect(offsetX + cell.col * cellSize, offsetY + cell.row * cellSize, cellSize - 2, cellSize - 2);
    }
}

void ShapeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << shape.id << shape.color;
        stream << shape.cells.size();
        for (const auto &cell : shape.cells) {
            stream << cell;
        }
        mimeData->setData("application/x-blockshape", data);
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
    }
}
