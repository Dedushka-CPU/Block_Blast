#pragma once
//вроде виджет  для фигур
#include <QWidget>
#include "blockshape.h"

class ShapeWidget : public QWidget {
    Q_OBJECT
public:
    ShapeWidget(const BlockShape &shape, QWidget *parent = nullptr);
    const BlockShape &getShape() const;
    void setColor(const QString &color);
    void updateAppearance();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    BlockShape shape;
};
