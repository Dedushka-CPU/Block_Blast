#pragma once

#include <QWidget>
#include "blockshape.h"

class ShapeWidget : public QWidget {
    Q_OBJECT
public:
    ShapeWidget(const BlockShape &shape, QWidget *parent = nullptr);
    const BlockShape &getShape() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    BlockShape shape;
};
