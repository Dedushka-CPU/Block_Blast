#include "fallingshapeswidget.h"
#include <QPainter>//для рисования используем
#include <QRandomGenerator>//рандомный генератор
#include <QTimerEvent>//штука для таймеров и тп
#include <QTransform>//для смещения фигуры имитирует падение

FallingShapesWidget::FallingShapesWidget(QWidget *parent) : QWidget(parent), timerId(0) {
    setAttribute(Qt::WA_TransparentForMouseEvents);//используем этот атрибут что бы фигуры не реагировали на клики мыши
    setStyleSheet("background: transparent;");//используем прозрачный цвет

    // ну тут просто задаем наши фигуры
    QVector<QVector<QPoint>> templates = {
        {{1,1}}, // 1 клетка
        {{0,0}, {0,1}}, // 2 клетки
        {{0,0}, {1,0}},
        {{0,0}, {0,1}, {0,2}}, // 3 клетки
        {{0,0}, {1,0}, {2,0}},
        {{0,0}, {1,1}, {2,2}},
        {{0,2}, {1,1}, {2,0}},
        {{0,0}, {0,1}, {1,1}},
        {{0,0}, {0,1}, {1,0}},
        {{0,0}, {1,0}, {1,1}},
        {{0,1}, {1,0}, {1,1}},
        {{0,0}, {0,1}, {1,0}, {1,1}}, // Квадрат
        {{0,0}, {0,1}, {0,2}, {1,1}}, // T-форма
        {{0,0}, {0,1}, {1,1}, {2,1}},
        {{0,0}, {0,1}, {1,0}, {2,0}},
        {{0,0}, {1,0}, {1,1}, {1,2}},
        {{1,0}, {1,1}, {1,2}, {0,2}},
        {{0,1}, {1,1}, {2,1}, {2,0}},
        {{0,0}, {1,0}, {2,0}, {2,1}},
        {{0,0}, {0,1}, {1,1}, {1,2}},
        {{0,2}, {0,1}, {1,1}, {1,0}},
        {{0,0}, {1,0}, {1,1}, {2,1}},
        {{0,1}, {1,1}, {1,0}, {2,0}},
        {{0,0}, {0,1}, {0,2}, {0,3}}, // линия гор 4
        {{0,0}, {1,0}, {2,0}, {3,0}}, // линия вертикальная 4
        {{0,0}, {0,1}, {0,2}, {1,2}, {2,2}}, // ~|
        {{0,0}, {0,1}, {0,2}, {1,0}, {2,0}}, // |~
        {{2,0}, {2,1}, {0,2}, {1,2}, {2,2}}, // |_
        {{2,0}, {2,1}, {0,0}, {1,0}, {2,2}}, // _|
        {{2,0}, {2,1}, {2,2}, {2,3}, {2,4}}, // линия гор 5
        {{0,2}, {1,2}, {2,2}, {3,2}, {4,2}}, // линия вертикальная 5
        {{0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2}, {2,0}, {2,1}, {2,2}}, // квадрат на 3 клетки
    };
    QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};//вектор цветов в которые красим фигуры

    // тут делаются фигуры
    for (int i = 0; i < 10; ++i) {//тут создаем 10 фигур которые будут на экране
        Shape shape;//создаем фигуру
        shape.cells = templates[QRandomGenerator::global()->bounded(templates.size())];//тут получаем рандомную фигуру
        int maxX = 0;
        int minX = 0;
        for (const auto &cell : shape.cells) {
            maxX = qMax(maxX, cell.x());
            minX = qMin(minX, cell.x());
        }
        int cellSize = 20;//размер падающей фигуры

        shape.x = QRandomGenerator::global()->bounded(minX * cellSize, width() - maxX * cellSize);
        // Фигуры появляются сверху, выше виджета
        shape.y = -cellSize * (maxX + 1); // Учитываем размер фигуры, чтобы она появлялась полностью выше экрана
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
                // Учитываем размер фигуры для корректного сброса по x
                int maxX = 0;
                int minX = 0;
                for (const auto &cell : shape.cells) {
                    maxX = qMax(maxX, cell.x());
                    minX = qMin(minX, cell.x());
                }
                int cellSize = 20;
                // Задаём x так, чтобы фигура помещалась в пределах экрана
                shape.x = QRandomGenerator::global()->bounded(minX * cellSize, width() - maxX * cellSize);
                // Сбрасываем y, чтобы фигура появлялась сверху
                shape.y = -cellSize * (maxX + 1); // Учитываем размер фигуры
                shape.speed = QRandomGenerator::global()->bounded(50, 150) / 100.0;
                shape.rotation = QRandomGenerator::global()->bounded(0, 360);
            }
        }
        update();
    }
}
