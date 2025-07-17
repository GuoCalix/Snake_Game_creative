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

// 初始化静态成员变量maxScore（游戏最高分）
int GameController::maxScore = 0;

/**
 * @brief GameController类构造函数
 * @param scene 游戏场景引用
 * @param parent 父对象指针
 * 初始化游戏控制器，设置游戏初始状态，显示玩家数量选择菜单，并安装事件过滤器
 */
GameController::GameController(QGraphicsScene &scene, QObject *parent) :
    QObject(parent),
    scene(scene),
    isPause(false),     // 游戏未暂停
    waitingForModeSelection(true),      // 等待移动模式选择
    waitingForNumplayerSelection(true),     // 等待玩家数量选择
    Numplayer(1),// 默认1名玩家
    currentMoveMode(FOUR_DIRECTIONS),// 默认四方向移动模式
    currentGameMode(CHALLENGE_MODE),     // 默认闯关游戏模式
    currentLevel(1),                    // 默认初始关卡1
    score(0)                            // 初始分数0
{
    showNumplayerSelection();       // 显示玩家数量选择菜单
    scene.installEventFilter(this);     // 为场景安装事件过滤器，捕获键盘事件
    timer.start(50);        // 启动定时器，每50ms触发一次场景更新
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));         // 连接定时器与场景更新
    isPause = false;        // 确保游戏处于运行状态
}

/**
 * @brief GameController类析构函数
 * 清理游戏资源（默认空实现，可根据需要添加资源释放逻辑）
 */
GameController::~GameController()
{
}

/**
 * @brief 显示游戏模式选择菜单（无限积分模式/闯关模式）
 * 清除之前的菜单元素，创建新的游戏模式选择文本项并添加到场景
 */
void GameController::showGameModeSelection()
{
    // 清除之前的菜单
    foreach (QGraphicsItem *item, menuItems) {
        scene.removeItem(item);
    }
    menuItems.clear();

    // 创建游戏模式选择标题
    QGraphicsTextItem *title = new QGraphicsTextItem("选择游戏模式");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);   // 居中显示
    scene.addItem(title);

    // 创建"无限积分模式"选项
    QGraphicsTextItem *infiniteItem = new QGraphicsTextItem("1. 无限积分模式");
    infiniteItem->setFont(QFont("Arial", 12));
    infiniteItem->setPos(-infiniteItem->boundingRect().width()/2, 0);
    scene.addItem(infiniteItem);

    // 创建"闯关模式"选项
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
    //创建选择菜单标题
    QGraphicsTextItem *title = new QGraphicsTextItem("选择玩家数量");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);
    scene.addItem(title);

     // 创建"1P"选项
    QGraphicsTextItem *onePlayer = new QGraphicsTextItem("1. 1P");
    onePlayer->setFont(QFont("Arial", 12));
    onePlayer->setPos(-onePlayer->boundingRect().width()/2, 0);
    scene.addItem(onePlayer);

    // 创建"2P"选项
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

/**
 * @brief 显示移动模式选择菜单（四方向/六方向）
 * 清除场景中现有元素，创建移动模式选择文本项并添加到场景
 */
void GameController::showModeSelection()
{
    scene.clear();  // 清空场景

    // 创建移动模式选择标题
    QGraphicsTextItem *title = new QGraphicsTextItem("选择移动模式");
    title->setFont(QFont("Arial", 16));
    title->setPos(-title->boundingRect().width()/2, -50);
    scene.addItem(title);

    // 创建"四方向"选项
    QGraphicsTextItem *fourDirItem = new QGraphicsTextItem("1. 四方向 (上下左右)");
    fourDirItem->setFont(QFont("Arial", 12));
    fourDirItem->setPos(-fourDirItem->boundingRect().width()/2, 0);
    scene.addItem(fourDirItem);

    // 创建"六方向"选项（含斜角移动）
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

/**
 * @brief 清除指定标题的菜单
 * @param menuTitle 要清除的菜单标题文本
 * 遍历菜单项，找到匹配标题的菜单并移除所有相关元素
 */
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
            delete item;    // 释放内存
        }
        menuItems.clear();  // 清空菜单项列表
    }
}

/**
 * @brief 开始游戏
 * 清除菜单，初始化游戏元素（蛇、食物、墙壁），设置游戏参数，启动游戏循环
 */
void GameController::startGame()
{
    // 清除菜单
    menuItems.clear();

    // 标记为已完成移动模式选择
    waitingForModeSelection = false;

    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 添加墙
    addWalls();

     // 创建关卡提升提示文本项
    levelUpHint = new QGraphicsTextItem();
    levelUpHint->setFont(QFont("Arial", 24, QFont::Bold));// 粗体显示
    levelUpHint->setDefaultTextColor(Qt::yellow);// 黄色文本
    levelUpHint->setZValue(10);  // 确保显示在最上层
    levelUpHint->setVisible(false);  // 默认隐藏
    scene.addItem(levelUpHint);

    // 初始化提示定时器
    hintTimer = new QTimer(this);
    hintTimer->setSingleShot(true);  // 只触发一次
    connect(hintTimer, &QTimer::timeout, this, [=]() {
        levelUpHint->setVisible(false);  // 定时隐藏提示
    });

    timer.start(50);    // 启动定时器，每50ms更新一次
    currentLevel = 1;   // 重置关卡为1

    // 根据移动模式创建蛇1（玩家1）
    snake1 = new Snake(*this, 1, currentMoveMode);
    
    scene.addItem(snake1);
    
    // 如果是2名玩家，创建蛇2（玩家2）
    if (Numplayer == 2) {
        snake2 = new Snake(*this, 2, currentMoveMode);
        scene.addItem(snake2);
    } else {
        snake2 = nullptr;// 否则蛇2为空
    }

     // 根据游戏模式初始化参数
    if (currentGameMode == CHALLENGE_MODE) {
        // 闯关模式：初始关卡1，设置关卡目标分数
        currentLevel = 1;
        score = 0;
        qDebug() << "进入闯关模式，当前关卡：" << currentLevel;
        addRandomWalls();       // 添加随机墙壁增加难度
    } else {
        // 无限模式：重置分数
        score = 0;
        qDebug() << "进入无限积分模式";
    }

    // 添加食物
    addNewFood();

    // 启动游戏循环（连接定时器与场景更新）
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    isPause = false;
}

/**
 * @brief 获取当前移动模式
 * @return 当前移动模式（四方向/六方向）
 */
MoveMode GameController::getMovemode() const{ return currentMoveMode; }

/**
 * @brief 处理蛇吃到食物的逻辑
 * @param food 被吃掉的食物
 * @param snake 吃到食物的蛇
 * 移除食物，添加新食物，增加分数，并在闯关模式下判断是否升级关卡
 */
void GameController::snakeAteFood(Food *food, Snake *snake)
{
    scene.removeItem(food);     // 从场景中移除食物
    delete food;    // 释放食物内存

    addNewFood();   // 添加新食物

    score += 10;    //增加分数
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

            // 关卡难度提升（如加速，添加随机墙壁）
            addRandomWalls();
            // 提高游戏速度（定时器间隔减小，最低10ms）
            timer.setInterval(qMax(10, 25 - (currentLevel-1)*2));
            qDebug() << "进入第" << currentLevel << "关，当前速度：" << timer.interval() << "ms";
        }
    }
}

/**
 * @brief 添加边界墙壁
 * 在场景四周添加墙壁，限制蛇的活动范围
 */
void GameController::addWalls()
{
    // 上边界墙壁
    Wall *topWall = new Wall(-2000, -1160, 4000, 40);
    scene.addItem(topWall);
    // 下边界墙壁
    Wall *bottomWall = new Wall(-2000, 1140, 4000, 40);
    scene.addItem(bottomWall);
    // 左边界墙壁
    Wall *leftWall = new Wall(-2020, -1160, 40, 2320);
    scene.addItem(leftWall);
    // 右边界墙壁
    Wall *rightWall = new Wall(1980, -1160, 40, 2320);
    scene.addItem(rightWall);
}

/**
 * @brief 添加随机墙壁（闯关模式下随关卡增加）
 * 根据当前关卡生成随机墙壁，确保不与蛇、食物重叠
 */
void GameController::addRandomWalls()
{
    // 根据当前关卡决定随机墙面数量（关卡越高墙面越多）
    int wallCount = currentLevel * 2; // 每关新增2个墙面
    qDebug() << "第" << currentLevel << "关，生成" << wallCount << "个随机墙面";

    for (int i = 0; i < wallCount; ++i) {
        qreal x, y;
        int attempts = 0;// 尝试次数计数器
        bool positionValid = true;// 位置是否有效标志

        // 循环生成有效位置（避免与蛇、食物、已有墙面重叠）
        do {
            // 1. 生成随机坐标（限制在场景围墙内部，预留边缘空间）
            x = -1800 + rand() % 3600; // 范围：[-1800, 1800)
            y = -900 + rand() % 1800;  // 范围：[-900, 900)

            // 2. 对齐到40x40网格（与蛇、食物的移动网格一致）
            x = qRound(x / 40) * 40;
            y = qRound(y / 40) * 40;

            // 3. 检测与蛇身重叠
            bool collideSnake1 = snake1->shape().contains(snake1->mapFromScene(QPointF(x, y)));
            //bool collideSnake2 = (Numplayer == 2) ? snake2->shape().contains(snake2->mapFromScene(QPointF(x, y))) : false;

            // 4. 检测与食物重叠
            bool collideFood = false;

            // 5. 检测与已有随机墙面重叠
            bool collideExistingWall = false;


            // 6. 验证位置有效性
            positionValid = !(collideSnake1  || collideFood || collideExistingWall);
            attempts++;
        } while (!positionValid && attempts < MAX_WALL_ATTEMPTS);

        // 生成墙面并添加到场景
        if (positionValid) {
            Wall *newWall = new Wall(x, y, WALL_SIZE, WALL_SIZE);
            scene.addItem(newWall);
            randomWalls.append(newWall);
        } else {
            qWarning() << "生成随机墙面失败，尝试次数超限";
        }
    }
}

/**
 * @brief 处理蛇吃到自己身体的情况（游戏结束）
 * @param snake 吃到自己的蛇
 */
void GameController::snakeAteItself(Snake *snake)
{
    loser = snake;// 记录失败者
    QTimer::singleShot(0, this, SLOT(gameOver()));// 立即触发游戏结束
}

/**
 * @brief 处理键盘按键事件（控制蛇的移动）
 * @param event 键盘事件
 * @param controller 游戏控制器指针
 * 根据当前移动模式和玩家数量，将按键映射为蛇的移动方向
 */
void GameController::handleKeyPressed(QKeyEvent *event, GameController *controller)
{
    // 如果游戏未暂停且是四方向移动模式
    if (!isPause && controller->getMovemode() == FOUR_DIRECTIONS) {
      switch (event->key()) {
            // 玩家1控制（方向键）
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
            // 玩家2控制（WASD键）
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
            // 空格键暂停/继续游戏
            case Qt::Key_Space:
                pause();
                break;
        }
    }
    // 如果游戏未暂停且是六方向移动模式（含斜角）
    else if (!isPause && controller->getMovemode() == SIX_DIRECTIONS) {
        switch (event->key()) {
            // 玩家1控制（W/E/A/D/Z/X键）
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
            // 玩家2控制（U/I/H/K/N/M键）
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

/**
 * @brief 处理蛇撞到墙壁的情况（游戏结束）
 * @param snake 撞到墙壁的蛇
 * @param wall 被撞到的墙壁
 */
void GameController::snakeHitWall(Snake *snake, Wall *wall)
{
    loser = snake;// 记录失败者
    QTimer::singleShot(0, this, SLOT(gameOver())); // 立即触发游戏结束
}

/**
 * @brief 添加新食物到场景
 * 随机生成食物位置，确保不与蛇的身体重叠
 */
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

/**
 * @brief 处理蛇与蛇之间的碰撞（仅2名玩家时有效）
 * @param snake1 蛇1
 * @param snake2 蛇2
 * 判断哪条蛇的头部撞到了对方的身体，确定失败者
 */
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

/**
 * @brief 游戏结束处理
 * 断开定时器与场景更新的连接，显示游戏结果对话框，根据用户选择重新开始或退出
 */
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
    // 2名玩家时，显示获胜玩家信息
    if (Numplayer == 2 ) {
        if (loser) {
            int winner = (loser->players() == 1) ? 2 : 1;// 失败者的对立玩家为获胜者
            resultText = QString("玩家%1获胜!").arg(winner);
        }
    }
    // 显示游戏结束对话框，询问是否重新开始
    if (QMessageBox::Yes == QMessageBox::information(NULL,
                            tr("Game Over"),
                            resultText + "\nAgain?",
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes)) {
        // 重新连接定时器与场景更新
        connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
        scene.clear();// 清空场景

        showNumplayerSelection();// 重新显示玩家数量选择菜单
        waitingForNumplayerSelection = true;

        isPause = true;// 暂停游戏

    } else {
        exit(0);
    }
}

/**
 * @brief 暂停游戏
 * 断开定时器与场景更新的连接，标记游戏为暂停状态
 */
void GameController::pause()
{
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));// 停止场景更新
    isPause = true;// 标记为暂停状态
    setResume();// 更新继续游戏按钮状态
}

/**
 * @brief 继续游戏
 * 重新连接定时器与场景更新的连接，标记游戏为运行状态
 */
void GameController::resume()
{
    connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));// 恢复场景更新
   isPause = false;// 标记为运行状态
   setResume();// 更新继续游戏按钮状态
}

/**
 * @brief 设置继续游戏按钮的启用状态
 * 根据游戏是否暂停，启用或禁用继续游戏按钮
 */
void GameController :: setResume(){
    if(isPause == true){
        resumeAction->setEnabled(true);// 暂停时启用继续按钮
    }else{
        resumeAction->setEnabled(false);// 运行时禁用继续按钮
    }
}

/**
 * @brief 事件过滤器（处理场景中的键盘事件）
 * @param object 事件源对象
 * @param event 事件
 * @return 是否处理了事件
 * 根据游戏当前状态（选择阶段/运行阶段）处理不同的键盘事件
 */
bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {// 处理键盘按下事件
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // 玩家数量选择阶段
    if (waitingForNumplayerSelection) {
        if (keyEvent->key() == Qt::Key_1) {
            Numplayer = 1;// 选择1名玩家
            waitingForNumplayerSelection = false;
            showModeSelection();  // 显示模式选择提示
            waitingForModeSelection = true;
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            Numplayer = 2;// 选择2名玩家
            waitingForNumplayerSelection = false;
            showModeSelection();// 显示移动模式选择
            waitingForModeSelection = true;
            return true;
        }
    }

    // 模式选择阶段
    if (waitingForModeSelection && Numplayer == 1) {
        if (keyEvent->key() == Qt::Key_1) {
            currentMoveMode = FOUR_DIRECTIONS;// 选择四方向模式
            emit moveModeSelected(FOUR_DIRECTIONS);// 发射移动模式选择信号
            waitingForModeSelection = false;
            waitingForGameModeSelection = true;// 进入游戏模式选择
            showGameModeSelection();// 显示游戏模式选择
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            currentMoveMode = SIX_DIRECTIONS;// 选择六方向模式
            emit moveModeSelected(SIX_DIRECTIONS);
            waitingForModeSelection = false;
            waitingForGameModeSelection = true;
            showGameModeSelection();
            return true;
        }
    }
     // 移动模式选择阶段（2名玩家时，选择后直接开始游戏）
    else if(waitingForModeSelection && Numplayer == 2){
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
    // 游戏模式选择阶段（1名玩家时，选择后开始游戏）
    if (waitingForGameModeSelection) {
        if (keyEvent->key() == Qt::Key_1) {
            currentGameMode = INFINITE_MODE;// 选择无限积分模式
            waitingForGameModeSelection = false;
            clearMenu("选择游戏模式");// 清除游戏模式选择菜单
            clearMenu("1. 无限积分模式");
            clearMenu("2. 闯关模式");

            emit gameModeSelected(INFINITE_MODE);// 发射游戏模式选择信号

            //startGame();  // 开始游戏
            return true;
        } else if (keyEvent->key() == Qt::Key_2) {
            currentGameMode = CHALLENGE_MODE;// 选择闯关模式
            waitingForGameModeSelection = false;
            clearMenu("选择游戏模式");
            clearMenu("1. 无限积分模式");
            clearMenu("2. 闯关模式");
            emit gameModeSelected(CHALLENGE_MODE);

            //startGame();  // 开始游戏
            return true;
        }
    }

    // 处理游戏控制
        if (!isPause) {
            handleKeyPressed(keyEvent, this);
        } else if (keyEvent->key() == Qt::Key_Space) {
            resume();// 暂停时按空格键继续
        }
        return true;
    } else {
        return QObject::eventFilter(object, event);// 其他事件交给父类处理
    }
}

/**
 * @brief 确定获胜者（仅2名玩家时有效）
 * 根据失败者信息确定获胜者，并发射游戏获胜信号
 */
void GameController::determineWinner() {
    if (loser == nullptr) return;
     // 失败方的对立玩家为获胜方
    int winner = (loser->players() == 1) ? 2 : 1;  // 失败方的对立玩家为获胜方
    emit gameWon(winner);  // 发射获胜信号
}
