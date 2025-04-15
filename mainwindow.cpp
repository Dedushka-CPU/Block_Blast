#include "mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QFont>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("BlockBlast");
    resize(800, 600);
    setStyleSheet("background-color: #1e1e2f;");

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Экран загрузки
    QWidget *loadingScreen = new QWidget();
    FallingShapesWidget *loadingBackground = new FallingShapesWidget(loadingScreen);
    loadingBackground->setGeometry(0, 0, 800, 600);
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
    menuBackground->setGeometry(0, 0, 800, 600);
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
    QPushButton *settingsButton = new QPushButton("Настройки", mainMenuScreen);
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
    settingsButton->setStyleSheet(buttonStyle);
    exitButton->setStyleSheet(buttonStyle);

    mainMenuLayout->addStretch();
    mainMenuLayout->addWidget(menuTitle);
    mainMenuLayout->addSpacing(40);
    mainMenuLayout->addWidget(startButton);
    mainMenuLayout->addSpacing(20);
    mainMenuLayout->addWidget(settingsButton);
    mainMenuLayout->addSpacing(20);
    mainMenuLayout->addWidget(exitButton);
    mainMenuLayout->addStretch();
    mainMenuLayout->setAlignment(Qt::AlignCenter);

    stackedWidget->addWidget(loadingScreen);
    stackedWidget->addWidget(mainMenuScreen);

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
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitGame);
}

void MainWindow::startGame() {
    // Заглушка для игрового экрана
}

void MainWindow::showSettings() {
    // Заглушка для настроек
}

void MainWindow::exitGame() {
    QApplication::quit();
}
