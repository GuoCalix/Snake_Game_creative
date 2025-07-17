#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QAction>
#include <QRandomGenerator>
#include "mainwindow.h"
#include "constants.h"
class QGraphicsScene;
class QKeyEvent;

class Snake;
class Food;
class Wall;

/**
 * @brief 游戏控制器类，负责管理游戏的整体逻辑、状态和交互
 * 协调蛇、食物、墙壁等游戏元素的交互，处理用户输入和游戏流程
 */

class GameController : public QObject
{
    Q_OBJECT
signals:
    void moveModeSelected(MoveMode moveMode);
    void gameModeSelected(GameMode gameMode);

    void gameWon(int winner);  // 发射获胜方信号（1或2）

public:
    GameController(QGraphicsScene &scene, QObject *parent = nullptr);
    ~GameController();

    void snakeAteFood(Food *food, Snake *snake);
    void snakeAteItself(Snake *snake);
    void snakeHitSnake(Snake *snake1, Snake *snake2);
    void snakeHitWall(Snake *snake, Wall *wall);
    QAction *getResmueAction(){ return resumeAction;}
    void setResumeAction(QAction* r){ resumeAction = r; }
    void showModeSelection();
    void showNumplayerSelection();
    void startGame();
    MoveMode getMovemode() const;
    GameMode getGameMode() const { return currentGameMode; }
    void showGameModeSelection();  // 显示游戏模式选择菜单
    void addWalls();
    void addRandomWalls(); //随机地形
    void clearMenu(const QString &menuTitle);
    void determineWinner();
public slots:
    void pause();
    void resume();
    void gameOver();
    bool eventFilter(QObject *object, QEvent *event);
protected:
    

private:
    void handleKeyPressed(QKeyEvent *event,GameController *controller);
    void addNewFood();
    void setResume();
    QList<Wall*> randomWalls; // 存储随机墙面
    const int WALL_SIZE = 40; // 单个墙面尺寸（与网格对齐）
    const int MAX_WALL_ATTEMPTS = 500; // 生成墙面的最大尝试次数（避免死循环）
    QAction * resumeAction;
    QTimer timer;
    QGraphicsScene &scene;
    Snake *snake1;
    Snake *snake2;
    bool isPause;
    MoveMode currentMoveMode;
    GameMode currentGameMode;  // 当前游戏模式
    int currentLevel;         // 当前关卡(仅闯关模式)
    int score;
    // 积分
    bool waitingForGameModeSelection;  // 游戏模式选择标志
    int Numplayer;
    bool waitingForModeSelection;
    bool waitingForNumplayerSelection; // 是否正在等待模式选择
    QList<QGraphicsItem*> menuItems; // 菜单项目列表

    QGraphicsTextItem* levelUpHint;  // 关卡提示文本
    QTimer* hintTimer;              // 控制提示消失的定时器

    Snake* loser = nullptr;  // 记录先碰撞的蛇（失败方

    static int maxScore;  // 最大积分


};

#endif // GAMECONTROLLER_H
