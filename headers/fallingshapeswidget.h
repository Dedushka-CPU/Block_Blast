#pragma once
#include <QWidget>
#include <QVector>

//это короче класс для фигур которые на фоне падают

class FallingShapesWidget : public QWidget {
    Q_OBJECT
public:
    explicit FallingShapesWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

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

    void resetShapes(const QVector<QVector<QPoint>> &templates, const QStringList &colors);
};

