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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), nextShapeId(0), score(0) {
    setWindowTitle("BlockBlast");
    resize(1000, 800);
    setStyleSheet("background-color: #1e1e2f;");

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Экран загрузки
    QWidget *loadingScreen = new QWidget();
    FallingShapesWidget *loadingBackground = new FallingShapesWidget(loadingScreen);
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
    FallingShapesWidget *menuBackground = new FallingShapesWidget(mainMenuScreen);
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
    FallingShapesWidget *gameBackground = new FallingShapesWidget(gameScreen);
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
    spawnLayout->setSpacing(20);
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
    screamerImage->setPixmap(QPixmap("/../../../assets/images/screamer.png").scaled(1000, 800, Qt::KeepAspectRatio));
    screamerImage->setAlignment(Qt::AlignCenter);
    screamerLayout->addWidget(screamerImage);
    screamerLayout->setAlignment(Qt::AlignCenter);

    screamSound = new QSoundEffect(this);
    screamSound->setSource(QUrl("/../../../assets/sounds/scream.mp3"));
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
    };
    static QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4", "#ffeead"};

    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i]) {
            spawnLayout->removeWidget(shapeWidgets[i]);
            shapeWidgets[i]->deleteLater();
            shapeWidgets[i] = nullptr;
        }
        int shapeIdx = QRandomGenerator::global()->bounded(shapeTemplates.size());
        int colorIdx = QRandomGenerator::global()->bounded(colors.size());
        shapes[i] = BlockShape(shapeTemplates[shapeIdx], colors[colorIdx], nextShapeId++);
        shapeWidgets[i] = new ShapeWidget(shapes[i], stackedWidget->widget(2));
        spawnLayout->insertWidget(i+1, shapeWidgets[i]);
    }
}

void MainWindow::handleDrop(int shapeId, const QPoint &pos) {
    for (int i = 0; i < 3; ++i) {
        if (shapeWidgets[i] && shapes[i].id == shapeId) {
            int row = pos.y() / 50;
            int col = pos.x() / 50;
            if (gameBoard->canPlaceShape(shapes[i], row, col)) {
                gameBoard->placeShape(shapes[i], row, col);
                spawnLayout->removeWidget(shapeWidgets[i]);
                shapeWidgets[i]->deleteLater();
                shapeWidgets[i] = nullptr;

                // Начисляем очки за размещение
                int cubeCount = shapes[i].cells.size(); // Предполагается, что shapes[i] хранит QVector<Cell>
                // Начисляем очки: 10 очков за каждый кубик
                updateScore(cubeCount * 10);
                //updateScore(10);

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
                if (!gameBoard->canPlaceAnyShape(shapes, shapeWidgets)) {
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

void MainWindow::endGame() {
    screamSound->play();
    stackedWidget->setCurrentIndex(5); // Показываем скример
    screamerTimer->start(4000); // 2 секунды
}
