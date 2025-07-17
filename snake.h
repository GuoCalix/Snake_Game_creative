#ifndef SNAKE_H
#define SNAKE_H

#include <QGraphicsItem>
#include <QRectF>
#include "constants.h"
class GameController;

/**
 * @brief 蛇类，继承自QGraphicsItem，实现贪吃蛇游戏中的蛇角色
 * 支持四方向和六方向移动模式，处理蛇的移动、碰撞检测和绘制
 */

class Snake : public QGraphicsItem
{
public:
    enum Direction {
        NoMove,
        East,       // 0度
        Northeast,  // 60度
        Northwest,   // 120度
        West,        // 180度
        Southwest,   // 240度
        Southeast,    // 300度
        MoveLeft, //左转90度
    	MoveRight, //右转90度
    	MoveUp,    //向上90度
    	MoveDown   //向下90度
    };

    Snake(GameController & controller, int player = 1, MoveMode moveode = FOUR_DIRECTIONS);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void setMoveDirection(Direction direction);
	Direction currentDirection();

    void turnEast();
    void turnNortheast();
    void turnNorthwest();
    void turnSouthwest();
    void turnSoutheast();
    void turnWest();


    MoveMode getMoveMode() const ;

    // 获取蛇头位置
    QPointF headPos() const { return head; }
    // 获取身体列表（包含所有尾部节点）
    const QList<QPointF>& body() const { return tail; }
    int players() const { return player; }

protected:
    void advance(int step);

private:
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void handleCollisions();

    QPointF        head;
    int            growing;
    int            speed;
    QList<QPointF> tail;
    int            tickCounter;
    Direction      moveDirection;
    GameController &controller;
	int 		   player;
    MoveMode	   moveMode;
};

#endif // SNAKE_H
