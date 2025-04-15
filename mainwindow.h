#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QSoundEffect>
#include <QTimer>
#include "fallingshapeswidget.h"
#include "gameboard.h"
#include "shapewidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startGame();
    void showSettings();
    void exitGame();
    void returnToMenu();
    void handleDrop(int shapeId, const QPoint &pos);
    void updateScore(int points);
    void endGame();

private:
    void generateShapes();

    QStackedWidget *stackedWidget;
    GameBoard *gameBoard;
    ShapeWidget *shapeWidgets[3];
    BlockShape shapes[3];
    QHBoxLayout *spawnLayout;
    QLabel *scoreLabel;
    QSoundEffect *screamSound;
    QTimer *screamerTimer;
    int nextShapeId;
    int score;
};

#endif // MAINWINDOW_H
