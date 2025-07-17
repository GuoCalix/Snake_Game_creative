#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hexagongrid.h"
#include "constants.h"

class QGraphicsScene;
class QGraphicsView;

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
    void handleGameModeSelected(GameMode mode);

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
