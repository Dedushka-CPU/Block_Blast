#pragma once
#include <QWidget>
#include <QVector>

class FallingShapesWidget : public QWidget {
    Q_OBJECT
public:
    explicit FallingShapesWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    struct Shape {
        QVector<QPoint> cells;
        double x, y;
        double speed;
        double rotation;
        QString color;
    };
    QVector<Shape> shapes;
    int timerId;
};

