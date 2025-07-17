#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hexagongrid.h"
#include "constants.h"

class QGraphicsScene;
class QGraphicsView;

/**
 * @brief 主窗口类，负责游戏界面的展示和用户交互
 * 继承自QMainWindow，包含游戏场景、视图和菜单栏等UI元素
 */

class GameController;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QGraphicsScene &scene ,QWidget *parent = 0,GameController *game = 0);
    ~MainWindow();
	void initSceneBackground(MoveMode mode);
	void initRectSceneBackground();
	void initHexSceneBackground();
	

private slots:
    void adjustViewSize();
	void newGame();
	void gameHelp();
	void about();
    void handleMoveModeSelected(MoveMode mode);

private:
	void createActions();
	void createMenus();

	void setupUI();

    QGraphicsScene &scene;
    QGraphicsView *view;

	GameController *game;

	QAction *newGameAction;
	QAction *pauseAction;
	QAction *resumeAction;
	QAction *exitAction;
	QAction *gameHelpAction;
	QAction *aboutAction;
	QAction *aboutQtAction;

    QGraphicsItem *backgroundItem = nullptr;

	
};

#endif // MAINWINDOW_H
