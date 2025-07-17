#include <QGraphicsView>
#include <QTimer>
#include <qaction.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <QPainterPath>
#include <QDebug>
#include <QScreen>

#include "constants.h"
#include "hexagongrid.h"
#include "gamecontroller.h"
#include "mainwindow.h"
#include <QIcon>
#include "rectgriditem.h"

MainWindow::MainWindow(QGraphicsScene &scene, QWidget *parent, GameController *game)
    : QMainWindow(parent),
      game(game),
      scene(scene),
      view(new QGraphicsView(&scene, this))
{   
    setCentralWidget(view);
    //   resize(600, 600);
    setFixedSize(1150, 1150);
    setWindowIcon(QIcon(":/images/snake_ico"));
	
	// 设置UI
    setupUI();
    
    // 连接信号和槽
    if (game) {
        connect(game, &GameController::moveModeSelected,
                this, &MainWindow::handleMoveModeSelected);

        connect(game, &GameController::gameModeSelected,
                this, &MainWindow::handleGameModeSelected);
    }

	createActions();
	createMenus();
    
    QTimer::singleShot(0, this, SLOT(adjustViewSize()));
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::adjustViewSize()
{
    view->fitInView(scene.sceneRect(), Qt::KeepAspectRatioByExpanding);
}

// 设置UI
void MainWindow::setupUI()
{
    // 设置窗口大小等
    setWindowTitle("贪吃蛇游戏");
    setMinimumSize(600, 600);
}

// 处理移动模式选择的槽函数
void MainWindow::handleMoveModeSelected(MoveMode mode)
{
    qDebug() << "收到模式选择信号，模式:" << (mode == FOUR_DIRECTIONS ? "四方向" : "六方向");
    
    // 根据选择的模式初始化场景背景
    initSceneBackground(mode);
    
    // 可以在这里执行其他与模式相关的操作

    game->startGame();
}

void MainWindow::handleGameModeSelected(GameMode mode)
{
    qDebug() << "游戏模式选择：" << (mode == INFINITE_MODE ? "无限积分模式" : "闯关模式");
    // 可根据游戏模式设置不同的场景样式（如关卡提示、分数显示）
}


// 初始化场景背景
void MainWindow::initSceneBackground(MoveMode mode)
{
    // 清除旧场景
    scene.clear();
	scene.setSceneRect(-50, -50, 100, 100);

    // 初始化新场景
    if (mode == FOUR_DIRECTIONS) {
        initRectSceneBackground();
    } else if (mode == SIX_DIRECTIONS) {
        initHexSceneBackground();
    }
}


void MainWindow::createActions()
{
	newGameAction = new QAction(tr("&New Game"), this);
	newGameAction->setShortcuts(QKeySequence::New);
	newGameAction->setStatusTip(tr("Start a new game"));
	connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);

	exitAction = new QAction(tr("&Exit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit the game"));
	connect(exitAction, &QAction::triggered, this, &MainWindow::close);

	pauseAction = new QAction(tr("&Pause"), this);
	pauseAction->setStatusTip(tr("Pause..."));
	connect(pauseAction, &QAction::triggered, game, &GameController::pause);

	resumeAction = new QAction(tr("&Resume"), this);
	resumeAction->setStatusTip(tr("Resume..."));
    resumeAction->setEnabled(false);
    game->setResumeAction(resumeAction);
    connect(resumeAction, &QAction::triggered, game, &GameController::resume);

	gameHelpAction = new QAction(tr("Game &Help"), this);
	gameHelpAction->setShortcut(tr("Ctrl+H"));
	gameHelpAction->setStatusTip(tr("Help on this game"));
	connect(gameHelpAction, &QAction::triggered, this, &MainWindow::gameHelp);

	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setStatusTip(tr("Show the application's about box"));
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

	aboutQtAction = new QAction(tr("About &Qt"), this);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAction, &QAction::triggered, qApp, QApplication::aboutQt);
}

void MainWindow::createMenus()
{
	QMenu *options = menuBar()->addMenu(tr("&Options"));
	options->addAction(newGameAction);
	options->addSeparator();
	options->addAction(pauseAction);
	options->addAction(resumeAction);
	options->addSeparator();
	options->addAction(exitAction);

	QMenu *help = menuBar()->addMenu(tr("&Help"));
	help->addAction(gameHelpAction);
	help->addAction(aboutAction);
	help->addAction(aboutQtAction);
}



void MainWindow::initRectSceneBackground()
{
    int rows = 100;
    int cols = 100;

    backgroundItem = new RectGridItem(rows, cols, TILE_SIZE);
    QRectF sceneRect = scene.sceneRect();
    backgroundItem->setPos(sceneRect.center() - backgroundItem->boundingRect().center());

    scene.addItem(backgroundItem);
}

void MainWindow::initHexSceneBackground()
{
	// 使用成员变量存储网格对象
	QRectF sceneRect = scene.sceneRect();
	const qreal gridWidth = sceneRect.width();
	const qreal gridHeight = sceneRect.height();


	HexagonGrid *hexagonGrid = new HexagonGrid(TILE_SIZE, gridWidth, gridHeight); // 设置父对象

	QPointF originalPos = sceneRect.center() - QPointF(gridWidth/2, gridHeight/2);

	int halfWidth = gridWidth / 2;
	int halfHeight = gridHeight / 2;

	hexagonGrid->setPos(originalPos + QPointF(halfWidth, halfHeight));

scene.addItem(hexagonGrid);
}

void MainWindow::newGame()
{
	QTimer::singleShot(0, game, SLOT(gameOver()));
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About this Game"), tr("<h2>Snake Game</h2>"
		"<p>Copyright &copy; XXX."
		"<p>This game is a small Qt application. It is based on the demo in the GitHub written by Devbean."));
}

void MainWindow::gameHelp()
{
	QMessageBox::about(this, tr("Game Help"), tr("Using direction keys to control the snake to eat the food"
		"<p>Space - pause & resume"));
}
