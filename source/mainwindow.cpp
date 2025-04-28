#include "../headers/mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), nextShapeId(0), score(0) {
    setWindowTitle("BlockBlast");
    resize(1000, 800);
    setStyleSheet("background-color: #1e1e2f;");

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Экран загрузки
    QWidget *loadingScreen = new QWidget();
    loadingBackground = new FallingShapesWidget(loadingScreen);
    loadingBackground->setGeometry(0, 0, 1000, 800);
    QVBoxLayout *loadingLayout = new QVBoxLayout(loadingScreen);

    QLabel *title = new QLabel("BlockBlast", loadingScreen);
    QFont titleFont("Arial", 48, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet(R"(
        color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff6b6b, stop:1 #4ecdc4);
        background: transparent;
    )");
    title->setAlignment(Qt::AlignCenter);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(3, 3);
    title->setGraphicsEffect(shadow);

    QProgressBar *progressBar = new QProgressBar(loadingScreen);
    progressBar->setRange(0, 100);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #4ecdc4;
            border-radius: 5px;
            background-color: #2a2a3d;
            height: 20px;
            text-align: center;
            color: white;
        }
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff6b6b, stop:1 #4ecdc4);
            border-radius: 3px;
        }
    )");
    progressBar->setValue(0);

    loadingLayout->addStretch();
    loadingLayout->addWidget(title);
    loadingLayout->addWidget(progressBar);
    loadingLayout->addStretch();
    loadingLayout->setAlignment(Qt::AlignCenter);

    // Главное меню
    QWidget *mainMenuScreen = new QWidget();
    menuBackground = new FallingShapesWidget(mainMenuScreen);
    menuBackground->setGeometry(0, 0, 1000, 800);
    QVBoxLayout *mainMenuLayout = new QVBoxLayout(mainMenuScreen);

    QLabel *menuTitle = new QLabel("BlockBlast", mainMenuScreen);
    menuTitle->setFont(titleFont);
    menuTitle->setStyleSheet(R"(
        color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff6b6b, stop:1 #4ecdc4);
        background: transparent;
    )");
    menuTitle->setAlignment(Qt::AlignCenter);
    QGraphicsDropShadowEffect *menuShadow = new QGraphicsDropShadowEffect();
    menuShadow->setBlurRadius(15);
    menuShadow->setColor(QColor(0, 0, 0, 160));
    menuShadow->setOffset(3, 3);
    menuTitle->setGraphicsEffect(menuShadow);

    QPushButton *startButton = new QPushButton("Начать игру", mainMenuScreen);
    QPushButton *exitButton = new QPushButton("Выход", mainMenuScreen);
    QString buttonStyle = R"(
        QPushButton {
            background-color: #4ecdc4;
            color: white;
            border: none;
            padding: 12px;
            font-size: 18px;
            border-radius: 8px;
            min-width: 220px;
        }
        QPushButton:hover {
            background-color: #ff6b6b;
        }
        QPushButton:pressed {
            background-color: #3daaa2;
        }
    )";
    startButton->setStyleSheet(buttonStyle);
    exitButton->setStyleSheet(buttonStyle);

    mainMenuLayout->addStretch();
    mainMenuLayout->addWidget(menuTitle);
    mainMenuLayout->addSpacing(40);
    mainMenuLayout->addWidget(startButton);
    mainMenuLayout->addSpacing(20);
    mainMenuLayout->addWidget(exitButton);
    mainMenuLayout->addStretch();
    mainMenuLayout->setAlignment(Qt::AlignCenter);

    // Экран игры
    QWidget *gameScreen = new QWidget();
    gameBackground = new FallingShapesWidget(gameScreen);
    gameBackground->setGeometry(0, 0, 1000, 800);
    QVBoxLayout *gameLayout = new QVBoxLayout(gameScreen);

    scoreLabel = new QLabel("Счет: 0", gameScreen);
    scoreLabel->setFont(QFont("Arial", 24));
    scoreLabel->setStyleSheet("color: #ffffff; background: transparent;");
    scoreLabel->setAlignment(Qt::AlignCenter);

    gameBoard = new GameBoard(gameScreen);
    gameBoard->setFixedSize(12 * 52, 12 * 52);

    QWidget *spawnArea = new QWidget(gameScreen);
    spawnLayout = new QHBoxLayout(spawnArea);
    spawnLayout->setSpacing(5);
    for (int i = 0; i < 3; ++i) {
        shapeWidgets[i] = nullptr;
    }
    spawnLayout->addStretch();
    spawnLayout->addStretch();

    QPushButton *backButton = new QPushButton("Вернуться в меню", gameScreen);
    backButton->setStyleSheet(buttonStyle);

    gameLayout->addStretch();
    gameLayout->addWidget(scoreLabel);
    gameLayout->addWidget(gameBoard, 0, Qt::AlignCenter);
    gameLayout->addWidget(spawnArea);
    gameLayout->addWidget(backButton);
    gameLayout->addStretch();
    gameLayout->setAlignment(Qt::AlignCenter);

    // Экран скримера
    QWidget *screamerScreen = new QWidget();
    screamerScreen->setStyleSheet("background-color: black;");
    QVBoxLayout *screamerLayout = new QVBoxLayout(screamerScreen);
    QLabel *screamerImage = new QLabel(screamerScreen);
    screamerImage->setPixmap(QPixmap("../../../assets/images/screamer.png").scaled(1000, 800, Qt::KeepAspectRatio));
    screamerImage->setAlignment(Qt::AlignCenter);
    screamerLayout->addWidget(screamerImage);
    screamerLayout->setAlignment(Qt::AlignCenter);

    screamSound = new QSoundEffect(this);
    screamSound->setSource(QUrl("../../../assets/sounds/scream.mp3"));
    screamerTimer = new QTimer(this);
    screamerTimer->setSingleShot(true);
    connect(screamerTimer, &QTimer::timeout, this, &MainWindow::returnToMenu);

    stackedWidget->addWidget(loadingScreen);
    stackedWidget->addWidget(mainMenuScreen);
    stackedWidget->addWidget(gameScreen);
    stackedWidget->addWidget(screamerScreen);

    // Анимация загрузки
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        static int progress = 0;
        progress += 5;
        progressBar->setValue(progress);
        if (progress >= 100) {
            timer->stop();
            stackedWidget->setCurrentIndex(1);
        }
    });
    timer->start(200);

    // Подключение кнопок
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitGame);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::returnToMenu);
    connect(gameBoard, &GameBoard::dropReceived, this, &MainWindow::handleDrop);
    connect(gameBoard, &GameBoard::scoreUpdated, this, &MainWindow::updateScore);
    connect(gameBoard, &GameBoard::linesCleared, this, &MainWindow::updateShapeColors);
}

void MainWindow::startGame() {
    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i]) {
            spawnLayout->removeWidget(shapeWidgets[i]);
            shapeWidgets[i]->deleteLater();
            shapeWidgets[i] = nullptr;
        }
    }
    score = 0;
    scoreLabel->setText("Счет: 0");
    generateShapes();
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::showSettings() {
    // Заглушка для настроек
}

void MainWindow::exitGame() {
    QApplication::quit();
}

void MainWindow::returnToMenu() {
    stackedWidget->setCurrentIndex(1);
    gameBoard->clearBoard();
}

void MainWindow::generateShapes() {
    static QVector<QVector<BlockShape::Cell>> shapeTemplates = {
        {{0,0}}, // 1 клетка
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
        {{0,0}, {0,1}, {0,2}, {0,3}}, //линия гор 4
        {{0,0}, {1,0}, {2,0}, {3,0}},  //линия вертикальная 4
        {{0,0}, {0,1}, {0,2}, {1,2}, {2,2}}, //~|
        {{0,0}, {0,1}, {0,2}, {1,0}, {2,0}}, //|~
        {{2,0}, {2,1}, {0,2}, {1,2}, {2,2}}, //|_
        {{2,0}, {2,1}, {0,0}, {1,0}, {2,2}}, //_|
        {{0,0}, {0,1}, {0,2}, {0,3}, {0,4}}, //линия гор 5
        {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}}, //линия вертикальная 5
        {{0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2}, {2,0}, {2,1}, {2,2}}, //квадрат на 3 клетки
        /*{{0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10},
            {1,0}, {1,1}, {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7}, {1,8}, {1,9}, {1,10},
            {2,0}, {2,1}, {2,2}, {2,3}, {2,4}, {2,5}, {2,6}, {2,7}, {2,8}, {2,9}, {2,10},
            {3,0}, {3,1}, {3,2}, {3,3}, {3,4}, {3,5}, {3,6}, {3,7}, {3,8}, {3,9}, {3,10},
            {4,0}, {4,1}, {4,2}, {4,3}, {4,4}, {4,5}, {4,6}, {4,7}, {4,8}, {4,9}, {4,10},
            {5,0}, {5,1}, {5,2}, {5,3}, {5,4}, {5,5}, {5,6}, {5,7}, {5,8}, {5,9}, {5,10},
            {6,0}, {6,1}, {6,2}, {6,3}, {6,4}, {6,5}, {6,6}, {6,7}, {6,8}, {6,9}, {6,10},
            {7,0}, {7,1}, {7,2}, {7,3}, {7,4}, {7,5}, {7,6}, {7,7}, {7,8}, {7,9}, {7,10},
            {8,0}, {8,1}, {8,2}, {8,3}, {8,4}, {8,5}, {8,6}, {8,7}, {8,8}, {8,9}, {8,10},
            {9,0}, {9,1}, {9,2}, {9,3}, {9,4}, {9,5}, {9,6}, {9,7}, {9,8}, {9,9}, {9,10},
            {10,0}, {10,1}, {10,2}, {10,3}, {10,4}, {10,5}, {10,6}, {10,7}, {10,8}, {10,9}, {10,10}}*/
    };
    static QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};

    bool allShapesUnplaceable = true; // Флаг для проверки, можно ли разместить хотя бы одну фигуру

    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i]) {
            spawnLayout->removeWidget(shapeWidgets[i]);
            shapeWidgets[i]->deleteLater();
            shapeWidgets[i] = nullptr;
        }

        bool canPlace = false;
        int attempts = 0;
        const int maxAttempts = 10;
        BlockShape newShape;

        while (!canPlace && attempts < maxAttempts) {
            int shapeIdx = QRandomGenerator::global()->bounded(shapeTemplates.size());
            int colorIdx = QRandomGenerator::global()->bounded(colors.size());
            newShape = BlockShape(shapeTemplates[shapeIdx], colors[colorIdx], nextShapeId++);
            canPlace = gameBoard->canPlaceShapeOnTheBoard(newShape);
            attempts++;
        }

        if (canPlace) {
            allShapesUnplaceable = false;
        }

        shapes[i] = newShape;
        shapeWidgets[i] = new ShapeWidget(shapes[i], stackedWidget->widget(2));
        spawnLayout->insertWidget(i + 1, shapeWidgets[i]);
        shapes[i].changeColor(canPlace);
        shapeWidgets[i]->setColor(shapes[i].color);
        shapeWidgets[i]->updateAppearance();
    }

    if (allShapesUnplaceable) {
        endGame();
    }
}

void MainWindow::updateShapeColors() {
    for (int j = 0; j < 3; ++j) {
        if (shapeWidgets[j] && !shapes[j].placed) {
            bool canPlace = gameBoard->canPlaceShapeOnTheBoard(shapes[j]);
            shapes[j].changeColor(canPlace);
            shapeWidgets[j]->setColor(shapes[j].color);
            shapeWidgets[j]->updateAppearance();
        }
    }
}

void MainWindow::handleDrop(int shapeId, const QPoint &pos) {
    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i] && shapes[i].id == shapeId) {
            int row = pos.y() / 50;
            int col = pos.x() / 50;
            if (gameBoard->canPlaceShape(shapes[i], row, col)) {
                gameBoard->placeShape(shapes[i], row, col);
                shapes[i].placed = true;
                spawnLayout->removeWidget(shapeWidgets[i]);
                shapeWidgets[i]->deleteLater();
                shapeWidgets[i] = nullptr;

                // Начисляем очки за размещение
                int cubeCount = shapes[i].cells.size();
                updateScore(cubeCount * 10);

                updateShapeColors();

                // Проверяем конец игры
                bool allPlaced = true;
                for (int j = 0; j < 3; ++j) {
                    if (shapeWidgets[j]) {
                        allPlaced = false;
                        break;
                    }
                }
                if (allPlaced) {
                    generateShapes();
                }

                // Проверяем, можно ли разместить оставшиеся фигуры
                bool allGray = true;
                bool hasRemainingShapes = false;
                for (int j = 0; j < 3; ++j) {
                    if (shapeWidgets[j] && !shapes[j].placed) {
                        hasRemainingShapes = true;
                        if (shapes[j].color != "#808080") {
                            allGray = false;
                            break;
                        }
                    }
                }

                if (hasRemainingShapes && allGray && !gameBoard->canPlaceAnyShape(shapes, shapeWidgets)) {
                    endGame();
                    return;
                }
            }
            break;
        }
    }
}



void MainWindow::updateScore(int points) {
    score += points;
    scoreLabel->setText(QString("Счет: %1").arg(score));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event); // Вызов базового класса

    if (gameBackground) {
        gameBackground->setGeometry(0, 0, event->size().width(), event->size().height());
    }

    if (menuBackground) {
        menuBackground->setGeometry(0, 0, event->size().width(), event->size().height());
    }

    if (loadingBackground) {
        loadingBackground->setGeometry(0, 0, event->size().width(), event->size().height());
    }
}

void MainWindow::endGame() {
    screamSound->play();
    stackedWidget->setCurrentIndex(5); // Показываем скример
    screamerTimer->start(4000); // 2 секунды
}

