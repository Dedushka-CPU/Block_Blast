#include "fallingshapeswidget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <QTransform>

FallingShapesWidget::FallingShapesWidget(QWidget *parent) : QWidget(parent), timerId(0) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background: transparent;");

    // Определяем шаблоны фигур
    QVector<QVector<QPoint>> templates = {
        {{0,0}}, // 1 клетка
        {{0,0}, {0,1}}, // 2 клетки горизонтально
        {{0,0}, {0,1}, {0,2}}, // 3 клетки горизонтально
        {{0,0}, {0,1}, {1,0}, {1,1}}, // Квадрат
        {{0,0}, {0,1}, {0,2}, {1,1}}, // T-форма
    };
    QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};

    // Создаем начальные фигуры
    for (int i = 0; i < 10; ++i) {
        Shape shape;
        shape.cells = templates[QRandomGenerator::global()->bounded(templates.size())];
        shape.x = QRandomGenerator::global()->bounded(width());
        shape.y = QRandomGenerator::global()->bounded(-200, 0);
        shape.speed = QRandomGenerator::global()->bounded(50, 150) / 100.0;
        shape.rotation = QRandomGenerator::global()->bounded(0, 360);
        shape.color = colors[QRandomGenerator::global()->bounded(colors.size())];
        shapes.append(shape);
    }

    timerId = startTimer(16); // ~60 FPS
}

void FallingShapesWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int cellSize = 20;
    for (const auto &shape : shapes) {
        painter.save();
        QTransform transform;
        transform.translate(shape.x, shape.y);
        transform.rotate(shape.rotation);
        painter.setTransform(transform);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(shape.color));
        for (const auto &cell : shape.cells) {
            painter.drawRect(cell.x() * cellSize, cell.y() * cellSize, cellSize - 2, cellSize - 2);
        }
        painter.restore();
    }
}

void FallingShapesWidget::timerEvent(QTimerEvent *event) {
    if (event->timerId() == timerId) {
        for (auto &shape : shapes) {
            shape.y += shape.speed;
            shape.rotation += shape.speed * 0.5;
            if (shape.y > height() + 100) {
                shape.y = QRandomGenerator::global()->bounded(-200, -50);
                shape.x = QRandomGenerator::global()->bounded(width());
                shape.speed = QRandomGenerator::global()->bounded(50, 150) / 100.0;
                shape.rotation = QRandomGenerator::global()->bounded(0, 360);
            }
        }
        update();
    }
}
