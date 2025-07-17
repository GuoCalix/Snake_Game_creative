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

/**
 * @brief 主窗口构造函数
 * @param scene 游戏场景
 * @param parent 父窗口
 * @param game 游戏控制器
 * 初始化主窗口，设置中央部件为QGraphicsView，连接游戏信号与槽，创建菜单和动作
 */
MainWindow::MainWindow(QGraphicsScene &scene, QWidget *parent, GameController *game)
    : QMainWindow(parent),
      game(game),
      scene(scene),
      view(new QGraphicsView(&scene, this))
{   
    setCentralWidget(view);// 设置视图为中央部件
    setFixedSize(1150, 1150);// 设置固定窗口大小
    setWindowIcon(QIcon(":/images/snake_ico"));// 设置窗口图标
	
	// 设置UI
    setupUI();
    
    // 连接游戏模式选择信号到相应的槽函数
    if (game) {
        connect(game, &GameController::moveModeSelected,
                this, &MainWindow::handleMoveModeSelected);
    }

    createActions();// 创建菜单项动作
    createMenus(); // 创建菜单栏
    
    // 延迟调整视图大小，确保场景已完全初始化
    QTimer::singleShot(0, this, SLOT(adjustViewSize()));
}

/**
 * @brief 主窗口析构函数
 */
MainWindow::~MainWindow()
{
    
}

/**
 * @brief 调整视图大小以适应场景
 * 确保场景内容完整显示在视图中，并保持宽高比
 */
void MainWindow::adjustViewSize()
{
    view->fitInView(scene.sceneRect(), Qt::KeepAspectRatioByExpanding);
}

/**
 * @brief 设置用户界面
 * 配置窗口基本属性，如标题和最小尺寸
 */
void MainWindow::setupUI()
{
    // 设置窗口大小等
    setWindowTitle("贪吃蛇游戏");
    setMinimumSize(600, 600);
}

/**
 * @brief 处理移动模式选择
 * @param mode 选择的移动模式（四方向或六方向）
 * 根据选择的模式初始化场景背景，并启动游戏
 */
void MainWindow::handleMoveModeSelected(MoveMode mode)
{
    qDebug() << "收到模式选择信号，模式:" << (mode == FOUR_DIRECTIONS ? "四方向" : "六方向");
    
    // 根据选择的模式初始化场景背景
    initSceneBackground(mode);
     // 启动游戏
    game->startGame();
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

/**
 * @brief 创建动作（菜单项）
 * 初始化游戏中的各种动作，如新建游戏、暂停、帮助等，并连接相应的槽函数
 */
void MainWindow::createActions()
{
    // 新建游戏动作
	newGameAction = new QAction(tr("&New Game"), this);
	newGameAction->setShortcuts(QKeySequence::New);
	newGameAction->setStatusTip(tr("Start a new game"));
	connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);

    // 退出动作
	exitAction = new QAction(tr("&Exit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit the game"));
	connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    // 暂停动作
	pauseAction = new QAction(tr("&Pause"), this);
	pauseAction->setStatusTip(tr("Pause..."));
	connect(pauseAction, &QAction::triggered, game, &GameController::pause);

    // 恢复动作
	resumeAction = new QAction(tr("&Resume"), this);
	resumeAction->setStatusTip(tr("Resume..."));
    resumeAction->setEnabled(false);
    game->setResumeAction(resumeAction);
    connect(resumeAction, &QAction::triggered, game, &GameController::resume);

    // 游戏帮助动作
	gameHelpAction = new QAction(tr("Game &Help"), this);
	gameHelpAction->setShortcut(tr("Ctrl+H"));
	gameHelpAction->setStatusTip(tr("Help on this game"));
	connect(gameHelpAction, &QAction::triggered, this, &MainWindow::gameHelp);

    // 关于动作
	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setStatusTip(tr("Show the application's about box"));
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    // 关于Qt动作
	aboutQtAction = new QAction(tr("About &Qt"), this);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAction, &QAction::triggered, qApp, QApplication::aboutQt);
}

/**
 * @brief 创建菜单栏
 * 将各种动作添加到菜单栏的相应菜单中
 */
void MainWindow::createMenus()
{
    // 创建选项菜单
	QMenu *options = menuBar()->addMenu(tr("&Options"));
	options->addAction(newGameAction);
	options->addSeparator();
	options->addAction(pauseAction);
	options->addAction(resumeAction);
	options->addSeparator();
	options->addAction(exitAction);

    // 创建帮助菜单
	QMenu *help = menuBar()->addMenu(tr("&Help"));
	help->addAction(gameHelpAction);
	help->addAction(aboutAction);
	help->addAction(aboutQtAction);
}

/**
 * @brief 初始化矩形网格背景
 * 创建矩形网格背景并添加到场景中
 */
void MainWindow::initRectSceneBackground()
{
    int rows = 100;
    int cols = 100;

    // 创建矩形网格项并居中放置
    backgroundItem = new RectGridItem(rows, cols, TILE_SIZE);
    QRectF sceneRect = scene.sceneRect();
    backgroundItem->setPos(sceneRect.center() - backgroundItem->boundingRect().center());

    scene.addItem(backgroundItem);// 添加到场景
}

/**
 * @brief 初始化六边形网格背景
 * 创建六边形网格背景并添加到场景中
 */
void MainWindow::initHexSceneBackground()
{
	// 使用成员变量存储网格对象
	QRectF sceneRect = scene.sceneRect();
	const qreal gridWidth = sceneRect.width();
	const qreal gridHeight = sceneRect.height();

    // 创建六边形网格并计算位置
	HexagonGrid *hexagonGrid = new HexagonGrid(TILE_SIZE, gridWidth, gridHeight); // 设置父对象

	QPointF originalPos = sceneRect.center() - QPointF(gridWidth/2, gridHeight/2);

	int halfWidth = gridWidth / 2;
	int halfHeight = gridHeight / 2;

     // 设置网格位置并添加到场景
	hexagonGrid->setPos(originalPos + QPointF(halfWidth, halfHeight));

scene.addItem(hexagonGrid);
}

/**
 * @brief 开始新游戏
 * 通过调用游戏控制器的gameOver()函数重新开始游戏
 */
void MainWindow::newGame()
{
	QTimer::singleShot(0, game, SLOT(gameOver()));
}

/**
 * @brief 显示关于对话框
 * 显示包含游戏信息的对话框
 */
void MainWindow::about()
{
	QMessageBox::about(this, tr("About this Game"), tr("<h2>Snake Game</h2>"
		"<p>Copyright &copy; XXX."
		"<p>This game is a small Qt application. It is based on the demo in the GitHub written by Devbean."));
}

/**
 * @brief 显示游戏帮助对话框
 * 显示包含游戏操作说明的对话框
 */
void MainWindow::gameHelp()
{
	QMessageBox::about(this, tr("Game Help"), tr("Using direction keys to control the snake to eat the food"
		"<p>Space - pause & resume"));
}
