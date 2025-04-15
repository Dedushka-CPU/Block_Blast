#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "fallingshapeswidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startGame();
    void showSettings();
    void exitGame();

private:
    QStackedWidget *stackedWidget;
};


