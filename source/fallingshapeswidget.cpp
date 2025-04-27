#include "../headers/fallingshapeswidget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <QTransform>

FallingShapesWidget::FallingShapesWidget(QWidget *parent) : QWidget(parent), timerId(0) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background: transparent;");

    // Шаблоны фигур
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
    QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};

    // Создание фигур
    shapes.resize(10); // Фиксируем 10 фигур
    resetShapes(templates, colors);

    timerId = startTimer(16); // ~60 FPS
}

void FallingShapesWidget::resetShapes(const QVector<QVector<QPoint>> &templates, const QStringList &colors) {
    int cellSize = 20;
    for (auto &shape : shapes) {
        shape.cells = templates[QRandomGenerator::global()->bounded(templates.size())];
        int maxX = 0, minX = 0, maxY = 0;
        for (const auto &cell : shape.cells) {
            maxX = qMax(maxX, cell.x());
            minX = qMin(minX, cell.x());
            maxY = qMax(maxY, cell.y());
        }
        // Учитываем текущую ширину виджета
        shape.x = QRandomGenerator::global()->bounded(minX * cellSize, width() - maxX * cellSize);
        shape.y = -cellSize * (maxY + 1); // Фигуры появляются сверху
        shape.speed = QRandomGenerator::global()->bounded(50, 150) / 100.0;
        shape.rotation = QRandomGenerator::global()->bounded(0, 360);
        shape.color = colors[QRandomGenerator::global()->bounded(colors.size())];
    }
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
        int cellSize = 20;
        for (auto &shape : shapes) {
            shape.y += shape.speed;
            shape.rotation += shape.speed * 0.5;
            int maxY = 0;
            for (const auto &cell : shape.cells) {
                maxY = qMax(maxY, cell.y());
            }
            // Проверяем, вышла ли фигура за нижнюю границу
            if (shape.y + maxY * cellSize > height() + cellSize) {
                int maxX = 0, minX = 0;
                for (const auto &cell : shape.cells) {
                    maxX = qMax(maxX, cell.x());
                    minX = qMin(minX, cell.x());
                }
                // Сбрасываем позицию с учетом текущей ширины
                shape.x = QRandomGenerator::global()->bounded(minX * cellSize, width() - maxX * cellSize);
                shape.y = -cellSize * (maxY + 1);
                shape.speed = QRandomGenerator::global()->bounded(50, 150) / 100.0;
                shape.rotation = QRandomGenerator::global()->bounded(0, 360);
            }
        }
        update();
    }
}

void FallingShapesWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // Обновляем позиции фигур при изменении размера окна
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
    QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};
    resetShapes(templates, colors);
}
