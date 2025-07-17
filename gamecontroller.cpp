#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QAction>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "gamecontroller.h"
#include "food.h"
#include "snake.h"
#include "mainwindow.h"
#include "wall.h"
#include "constants.h"

int GameController::maxScore = 0;

GameController::GameController(QGraphicsScene &scene, QObject *parent) :
    QObject(parent),
    scene(scene),
    isPause(false),
    waitingForModeSelection(true),
    waitingForNumplayerSelection(true),
    Numplayer(1),
    currentMoveMode(FOUR_DIRECTIONS),
    currentGameMode(INFINITE_MODE),     // 新增
    currentLevel(1),                    // 新增
    score(0)                            // 新增
{
  	showNumplayerSelection();
    scene.installEventFilter(this);
    timer.start(50);
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
   	isPause = false;
}

GameController::~GameController()
{
}

void GameController::showGameModeSelection()
{
    // 清除之前的菜单
    foreach (QGraphicsItem *item, menuItems) {
        scene.removeItem(item);
    }
    menuItems.clear();

    // 创建游戏模式选择菜单
    QGraphicsTextItem *title = new QGraphicsTextItem("选择游戏模式");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);
    scene.addItem(title);

    QGraphicsTextItem *infiniteItem = new QGraphicsTextItem("1. 无限积分模式");
    infiniteItem->setFont(QFont("Arial", 12));
    infiniteItem->setPos(-infiniteItem->boundingRect().width()/2, 0);
    scene.addItem(infiniteItem);

    QGraphicsTextItem *challengeItem = new QGraphicsTextItem("2. 闯关模式");
    challengeItem->setFont(QFont("Arial", 12));
    challengeItem->setPos(-challengeItem->boundingRect().width()/2, 30);
    scene.addItem(challengeItem);

    // 存储菜单项
    menuItems.append(title);
    menuItems.append(infiniteItem);
    menuItems.append(challengeItem);

    // 等待用户选择
    waitingForGameModeSelection = true;
}


void GameController::showNumplayerSelection()
{
  //创建选择菜单
    QGraphicsTextItem *title = new QGraphicsTextItem("选择玩家数量");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);
    scene.addItem(title);

    QGraphicsTextItem *onePlayer = new QGraphicsTextItem("1. 1P");
    onePlayer->setFont(QFont("Arial", 12));
    onePlayer->setPos(-onePlayer->boundingRect().width()/2, 0);
    scene.addItem(onePlayer);

    QGraphicsTextItem *twoPlayer = new QGraphicsTextItem("2. 2P");
    twoPlayer->setFont(QFont("Arial", 12));
    twoPlayer->setPos(-twoPlayer->boundingRect().width()/2, 30);
    scene.addItem(twoPlayer);

    // 存储菜单项引用
    menuItems.append(title);
    menuItems.append(onePlayer);
    menuItems.append(twoPlayer);

    // 等待用户选择
    waitingForNumplayerSelection = true;
}


void GameController::showModeSelection()
{
    scene.clear();
    // 创建选择菜单
    QGraphicsTextItem *title = new QGraphicsTextItem("选择移动模式");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);
    scene.addItem(title);

    QGraphicsTextItem *fourDirItem = new QGraphicsTextItem("1. 四方向 (上下左右)");
    fourDirItem->setFont(QFont("Arial", 12));
    fourDirItem->setPos(-fourDirItem->boundingRect().width()/2, 0);
    scene.addItem(fourDirItem);

    QGraphicsTextItem *sixDirItem = new QGraphicsTextItem("2. 六方向 (含斜角)");
    sixDirItem->setFont(QFont("Arial", 12));
    sixDirItem->setPos(-sixDirItem->boundingRect().width()/2, 30);
    scene.addItem(sixDirItem);

    // 存储菜单项引用
    menuItems.append(title);
    menuItems.append(fourDirItem);
    menuItems.append(sixDirItem);

    // 等待用户选择
    waitingForModeSelection = true;

}

void GameController::clearMenu(const QString &menuTitle)
{
    // 遍历所有菜单项，查找标题匹配的菜单
    QList<QGraphicsItem*> itemsToRemove;

    // 检查是否有标题项
    bool isMenuFound = false;
    for (QGraphicsItem *item : menuItems) {
        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem && textItem->toPlainText() == menuTitle) {
            isMenuFound = true;
            break;
        }
    }

    // 如果找到菜单，则清除该菜单的所有项
    if (isMenuFound) {
        for (QGraphicsItem *item : menuItems) {
            scene.removeItem(item);
            delete item;
        }
        menuItems.clear();
    }
}

void GameController::startGame()
{
    // 清除菜单
    menuItems.clear();

    waitingForModeSelection = false;

    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 添加墙
    addWalls();

    levelUpHint = new QGraphicsTextItem();
    levelUpHint->setFont(QFont("Arial", 24, QFont::Bold));
    levelUpHint->setDefaultTextColor(Qt::yellow);
    levelUpHint->setZValue(10);  // 确保显示在最上层
    levelUpHint->setVisible(false);  // 默认隐藏
    scene.addItem(levelUpHint);

    // 初始化提示定时器
    hintTimer = new QTimer(this);
    hintTimer->setSingleShot(true);  // 只触发一次
    connect(hintTimer, &QTimer::timeout, this, [=]() {
        levelUpHint->setVisible(false);  // 定时隐藏提示
    });

    // 根据模式创建蛇
    snake1 = new Snake(*this, 1, currentMoveMode);
    
    scene.addItem(snake1);
    

    if (Numplayer == 2) {
        snake2 = new Snake(*this, 2, currentMoveMode);
        scene.addItem(snake2);
    } else {
        snake2 = nullptr;
    }

    if (currentGameMode == CHALLENGE_MODE) {
        // 闯关模式：初始关卡1，设置关卡目标分数
        currentLevel = 1;
        score = 0;
        qDebug() << "进入闯关模式，当前关卡：" << currentLevel;
        // 可在此处添加关卡专属障碍物
    } else {
        // 无限模式：重置分数
        score = 0;
        qDebug() << "进入无限积分模式";
    }

    // 添加食物
    addNewFood();

    // 根据模式调整游戏参数

    // 启动游戏循环
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    isPause = false;
}

MoveMode GameController::getMovemode() const{ return currentMoveMode; }


void GameController::snakeAteFood(Food *food, Snake *snake)
{
    scene.removeItem(food);
    delete food;

    addNewFood();

    //增加分数
    score += 10;
    qDebug() << "当前分数：" << score;

    //闯关模式逻辑达到目标分数晋级
    if (currentGameMode == CHALLENGE_MODE) {
        if (score > maxScore) {
            maxScore = score;  // 更新记录
            qDebug() << "更新最高分：" << maxScore;  // 调试信息
        }
        int targetScore = currentLevel * 50;  // 每关目标分数递增
        if (score >= targetScore) {

            currentLevel++;
            // 更新提示文本
            levelUpHint->setPlainText(QString("恭喜！进入第 %1 关").arg(currentLevel));
            // 计算屏幕中心位置（假设场景中心为原点）
            levelUpHint->setPos(-levelUpHint->boundingRect().width()/2, -50);
            // 显示提示并启动定时器（3秒后隐藏）
            levelUpHint->setVisible(true);
            hintTimer->start(3000);  // 3000毫秒 = 3秒

            // 关卡难度提升（如加速）
            timer.setInterval(qMax(10, 25 - (currentLevel-1)*2));
            qDebug() << "进入第" << currentLevel << "关，当前速度：" << timer.interval() << "ms";
        }
    }
}

void GameController::addWalls()
{
    Wall *topWall = new Wall(-2000, -1160, 4000, 40);
    scene.addItem(topWall);
    Wall *bottomWall = new Wall(-2000, 1140, 4000, 40);
    scene.addItem(bottomWall);
    Wall *leftWall = new Wall(-2020, -1160, 40, 2320);
    scene.addItem(leftWall);
    Wall *rightWall = new Wall(1980, -1160, 40, 2320);
    scene.addItem(rightWall);
}

void GameController::snakeAteItself(Snake *snake)
{
    loser = snake;
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

void GameController::handleKeyPressed(QKeyEvent *event, GameController *controller)
{
    if (!isPause && controller->getMovemode() == FOUR_DIRECTIONS) {
      switch (event->key()) {
            case Qt::Key_Left:
                snake1->setMoveDirection(Snake::MoveLeft);
                break;
            case Qt::Key_Right:
                snake1->setMoveDirection(Snake::MoveRight);
                break;
            case Qt::Key_Up:
                snake1->setMoveDirection(Snake::MoveUp);
                break;
            case Qt::Key_Down:
                snake1->setMoveDirection(Snake::MoveDown);
                break;
            case Qt::Key_A:
                if (snake2) snake2->setMoveDirection(Snake::MoveLeft);
                break;
            case Qt::Key_D:
                if (snake2) snake2->setMoveDirection(Snake::MoveRight);
                break;
            case Qt::Key_W:
                if (snake2) snake2->setMoveDirection(Snake::MoveUp);
                break;
            case Qt::Key_S:
                if (snake2) snake2->setMoveDirection(Snake::MoveDown);
                break;
            case Qt::Key_Space:
                pause();
                break;
        }
    }else if (!isPause && controller->getMovemode() == SIX_DIRECTIONS) {
        switch (event->key()) {
          case Qt::Key_W:
            snake1->setMoveDirection(Snake::Northwest);
            break;
          case Qt::Key_E:
            snake1->setMoveDirection(Snake::Northeast);
            break;
          case Qt::Key_A:
            snake1->setMoveDirection(Snake::West);
            break;
          case Qt::Key_D:
            snake1->setMoveDirection(Snake::East);
            break;
          case Qt::Key_Z:
            snake1->setMoveDirection(Snake::Southwest);
            break;
          case Qt::Key_X:
            snake1->setMoveDirection(Snake::Southeast);
            break;
          case Qt::Key_U:
            if (snake2) snake2->setMoveDirection(Snake::Northwest);
            break;
          case Qt::Key_I:
            if (snake2) snake2->setMoveDirection(Snake::Northeast);
            break;
          case Qt::Key_H:
            if (snake2) snake2->setMoveDirection(Snake::West);
            break;
          case Qt::Key_K:
            if (snake2) snake2->setMoveDirection(Snake::East);
            break;
          case Qt::Key_N:
            if(snake2) snake2->setMoveDirection(Snake::Southwest);
            break;
          case Qt::Key_M:
            if(snake2) snake2->setMoveDirection(Snake::Southeast);
            break;
        }
    }

    else resume();
}

void GameController::snakeHitWall(Snake *snake, Wall *wall)
{
    loser = snake;
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

void GameController::addNewFood()
{
    qreal x, y;
    const int MAX_ATTEMPTS = 300;  // 最大尝试次数，避免无限循环
    int attempts = 0;

    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));

    do {
        // 生成随机位置（避开地图边缘和中心区域）
        x = -1900 + rand() % 3800;
        y = -1000 + rand() % 2000;

        // 对齐到网格
        x = qRound(x / 40) * 40;
        y = qRound(y / 40) * 40;

        attempts++;
    } while (
        // 检查与蛇1重叠
        snake1->shape().contains(snake1->mapFromScene(QPointF(x, y))) ||
        // 检查与蛇2重叠

        (Numplayer == 2 && snake2->shape().contains(snake2->mapFromScene(QPointF(x, y))))
         //||
        //!scene.items(QRectF(x, y, 10, 10)).isEmpty()
    );

    // 创建并添加新食物
    Food *newFood = new Food(x, y);
    scene.addItem(newFood);
}

void GameController::snakeHitSnake(Snake *snake1, Snake *snake2)
{   
    if (Numplayer == 2) {
        // 检测蛇A的头部是否撞到蛇B的身体
        bool isASnakeHeadHitB = snake1->body().contains(snake2->headPos());
        // 检测蛇B的头部是否撞到蛇A的身体
        bool isBSnakeHeadHitA = snake2->body().contains(snake1->headPos());

        // 情况1：蛇A主动撞蛇B的身体 → 蛇A输
        if (isASnakeHeadHitB) {
            loser = snake2;
        }
        // 情况2：蛇B主动撞蛇A的身体 → 蛇B输
        else if (isBSnakeHeadHitA) {
            loser = snake1;
        }
        QTimer::singleShot(0, this, SLOT(gameOver()));
    } else { return; }
}

void GameController::gameOver()
{
    disconnect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));

    QString resultText;
    if (currentGameMode == CHALLENGE_MODE) {
        // 新增：拼接最高分信息，若当前分数刷新记录则提示
        QString recordInfo;
        if (score > maxScore) {
            maxScore = score;  // 确保最终分数为最高分
            recordInfo = QString("\n恭喜!刷新最高分：%1").arg(maxScore);
        } else {
            recordInfo = QString("\n本次最高分:%1").arg(maxScore);
        }
        // 结果文本包含关卡、最终分数和最高分
        resultText = QString("游戏结束！当前关卡：%1, 最终分数：%2%3")
                        .arg(currentLevel)
                        .arg(score)
                        .arg(recordInfo);
    } else {
        resultText = QString("游戏结束！最终分数：%1").arg(score);
    }
    if (Numplayer == 2 ) {
        if (loser) {
            int winner = (loser->players() == 1) ? 2 : 1;
            resultText = QString("玩家%1获胜!").arg(winner);
        }
    }
    if (QMessageBox::Yes == QMessageBox::information(NULL,
                            tr("Game Over"),
                            resultText + "\nAgain?",
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes)) {
        connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
        scene.clear();

        showNumplayerSelection();
        waitingForNumplayerSelection = true;

        isPause = true;

        //showModeSelection();

        //waitingForModeSelection = true;
        //isPause = true;

        //snake1 = new Snake(*this, 1);
        //snake2 = new Snake(*this, 2);
        //scene.addItem(snake1);
        //scene.addItem(snake2);
        //addNewFood();
        //addWalls();
    } else {
        exit(0);
    }
}

void GameController::pause()
{
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));
    isPause = true;
    setResume();
}


void GameController::resume()
{
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
   isPause = false;
   setResume();
}
void GameController :: setResume(){
    if(isPause == true){
        resumeAction->setEnabled(true);
    }else{
        resumeAction->setEnabled(false);
    }
}

bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // 玩家数量选择阶段
    if (waitingForNumplayerSelection) {
        if (keyEvent->key() == Qt::Key_1) {
            Numplayer = 1;
            waitingForNumplayerSelection = false;
            showModeSelection();  // 显示模式选择提示
            waitingForModeSelection = true;
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            Numplayer = 2;
            waitingForNumplayerSelection = false;
            showModeSelection();
            waitingForModeSelection = true;
            return true;
        }
    }

    // 模式选择阶段
    if (waitingForModeSelection && Numplayer == 1) {
        if (keyEvent->key() == Qt::Key_1) {
            currentMoveMode = FOUR_DIRECTIONS;
            emit moveModeSelected(FOUR_DIRECTIONS);
            waitingForModeSelection = false;
            waitingForGameModeSelection = true;
            showGameModeSelection();
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            currentMoveMode = SIX_DIRECTIONS;
            emit moveModeSelected(SIX_DIRECTIONS);
            waitingForModeSelection = false;
            waitingForGameModeSelection = true;
            showGameModeSelection();
            return true;
        }
    }else if(waitingForModeSelection && Numplayer == 2){
        if (keyEvent->key() == Qt::Key_1) {
            currentMoveMode = FOUR_DIRECTIONS;
            emit moveModeSelected(FOUR_DIRECTIONS);
            waitingForModeSelection = false;
            return true;
        }else if (keyEvent->key() == Qt::Key_2) {
            currentMoveMode = SIX_DIRECTIONS;
            emit moveModeSelected(SIX_DIRECTIONS);
            waitingForModeSelection = false;
            return true;
        }
    }

    if (waitingForGameModeSelection) {
        if (keyEvent->key() == Qt::Key_1) {
            currentGameMode = INFINITE_MODE;
            waitingForGameModeSelection = false;
            clearMenu("选择游戏模式");
            clearMenu("1. 无限积分模式");
            clearMenu("2. 闯关模式");

            emit gameModeSelected(INFINITE_MODE);

            startGame();  // 开始游戏
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            currentGameMode = CHALLENGE_MODE;
            waitingForGameModeSelection = false;
            clearMenu("选择游戏模式");
            clearMenu("1. 无限积分模式");
            clearMenu("2. 闯关模式");
            emit gameModeSelected(CHALLENGE_MODE);

            startGame();  // 开始游戏
            return true;
        }
    }

    // 处理游戏控制
        if (!isPause) {
            handleKeyPressed(keyEvent, this);
        } else if (keyEvent->key() == Qt::Key_Space) {
            resume();
        }
        return true;
    } else {
        return QObject::eventFilter(object, event);
    }
}

void GameController::determineWinner() {
    if (loser == nullptr) return;

    int winner = (loser->players() == 1) ? 2 : 1;  // 失败方的对立玩家为获胜方
    emit gameWon(winner);  // 发射获胜信号
}
