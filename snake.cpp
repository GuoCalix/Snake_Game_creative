#include <QPainter>

#include "constants.h"
#include "gamecontroller.h"
#include "snake.h"
#include "wall.h"

// 蛇身体单元的大小（与瓦片大小一致）
static const qreal SNAKE_SIZE = TILE_SIZE;

/**
 * @brief 蛇类构造函数
 * @param controller 游戏控制器引用
 * @param player 玩家编号（1或2）
 * @param movemode 移动模式（四方向或六方向）
 * 初始化蛇的初始位置、属性，并设置玩家专属颜色标识
 */
Snake::Snake(GameController &controller, int player, MoveMode movemode) :
    head(0,0),      // 蛇头初始位置
    growing(1),     // 初始长度增长值（控制初始长度）
    speed(5),       // 移动速度（数值越大速度越慢）
    moveDirection(NoMove),      // 初始无移动方向
    controller(controller),     // 游戏控制器引用
    player(player),     // 玩家编号
    moveMode(movemode)      // 移动模式
{

    setZValue(1);   // 设置绘制层级，确保蛇显示在背景之上

    // 根据玩家编号设置初始位置（避免重叠)
    if (player == 1) {
        head = QPointF(500, -500).toPoint();  // 对齐到网格
    } else if (player == 2) {
        head = QPointF(-500, 500).toPoint();
    }
    // 确保坐标是TILE_SIZE的整数倍
    head.rx() = qRound(head.x() / 40) * 40;
    head.ry() = qRound(head.y() / 40) * 40;

    setPos(head);   // 设置蛇在场景中的初始位置
    tail.clear();   // 清空蛇尾（初始状态无尾部）

}

/**
 * @brief 获取蛇的边界矩形（用于碰撞检测和绘制范围）
 * @return 包含蛇头和所有尾部的矩形区域
 */
QRectF Snake::boundingRect() const
{
    qreal minX = head.x();  // 初始最小X坐标（蛇头X）
    qreal minY = head.y();  // 初始最小Y坐标（蛇头Y）
    qreal maxX = head.x();  // 初始最大X坐标（蛇头X）
    qreal maxY = head.y();  // 初始最大Y坐标（蛇头Y）

    // 遍历所有尾部单元，更新边界坐标
    foreach (QPointF p, tail) {
        maxX = p.x() > maxX ? p.x() : maxX;
        maxY = p.y() > maxY ? p.y() : maxY;
        minX = p.x() < minX ? p.x() : minX;
        minY = p.y() < minY ? p.y() : minY;
    }

    // 将场景坐标转换为本地坐标
    QPointF tl = mapFromScene(QPointF(minX, minY));
    QPointF br = mapFromScene(QPointF(maxX, maxY));

     // 计算并返回包含整个蛇的边界矩形
    QRectF bound = QRectF(tl.x(),  // x
                          tl.y(),  // y
                          br.x() - tl.x() + SNAKE_SIZE,      // width
                          br.y() - tl.y() + SNAKE_SIZE       //height
                          );
    return bound;
}

/**
 * @brief 获取蛇的形状路径（用于精确碰撞检测）
 * @return 包含蛇头和所有尾部的形状路径
 */
QPainterPath Snake::shape() const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);  // 设置填充规则为缠绕填充

    // 添加蛇头的矩形形状
    path.addRect(QRectF(0, 0, SNAKE_SIZE, SNAKE_SIZE));

    // 添加所有尾部单元的矩形形状
    foreach (QPointF p, tail) {
        QPointF itemp = mapFromScene(p);    // 转换为本地坐标
        path.addRect(QRectF(itemp.x(), itemp.y(), SNAKE_SIZE, SNAKE_SIZE));
    }

    return path;
}

/**
 * @brief 绘制蛇
 * @param painter 绘图工具
 * @param 样式选项（未使用）
 * @param 父窗口部件（未使用）
 * 根据玩家编号使用不同颜色填充蛇的形状（玩家1黄色，玩家2蓝色）
 */
void Snake::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();    // 保存当前绘图状态
    if (player == 1) {
        painter->fillPath(shape(), Qt::yellow);// 玩家1的蛇用黄色填充
    } else {
        painter->fillPath(shape(), Qt::blue); // 玩家2的蛇用蓝色填充
    }
    painter->restore(); // 恢复绘图状态
}

/**
 * @brief 设置蛇的移动方向
 * @param direction 目标移动方向
 * 防止蛇直接反向移动（如向左移动时不能直接向右），六方向模式下增加斜向移动的限制
 */
void Snake::setMoveDirection(Direction direction)
{
    // 四方向移动限制（防止直接反向）
    if (moveDirection == MoveLeft && direction == MoveRight)
        return;
    if (moveDirection == MoveRight && direction == MoveLeft)
        return;
    if (moveDirection == MoveUp && direction == MoveDown)
        return;
    if (moveDirection == MoveDown && direction == MoveUp)
        return;
    if (moveDirection == East && direction == West)
      	return;
    if (moveDirection == Northwest && direction == Southeast)
        return;
    if (moveDirection == Southeast && direction == Southwest)
      	return;
    moveDirection = direction;
}

/**
 * @brief 获取当前移动方向
 * @return 当前移动方向
 */
Snake::Direction Snake::currentDirection()
{
	return moveDirection;
}

/**
 * @brief 蛇的帧更新函数（QGraphicsItem的虚函数）
 * @param step 步骤（0为准备阶段，1为更新阶段）
 * 控制蛇的移动逻辑，根据速度更新位置，处理身体增长和碰撞检测
 */
void Snake::advance(int step)
{
    if (!step) {
        return;
    }
    if (tickCounter++ % speed != 0) {
        return;
    }
    if (moveDirection == NoMove) {
        return;
    }

    if (growing > 0) {
		QPointF tailPoint = head;
        tail << tailPoint;
        growing -= 1;
    } else {
        tail.removeFirst();
        tail << head;
    }

    switch (moveDirection) {
        case MoveLeft:
            moveLeft();
            break;
        case MoveRight:
            moveRight();
            break;
        case MoveUp:
            moveUp();
            break;
        case MoveDown:
            moveDown();
            break;
    	case East:
        	head.rx() += HEX_X;
        	break;
    	case Northeast:
        	head.rx() += HEX_X/2;
        	head.ry() -= HEX_Y;
        	break;
    	case Northwest:
        	head.rx() -= HEX_X/2;
        	head.ry() -= HEX_Y;
        	break;
   	 	case West:
        	head.rx() -= HEX_X;
        	break;
    	case Southwest:
        	head.rx() -= HEX_X/2;
        	head.ry() += HEX_Y;
        	break;
    	case Southeast:
        	head.rx() += HEX_X/2;
        	head.ry() += HEX_Y;
        	break;
    }

    setPos(head);
    handleCollisions();
}

void Snake::moveLeft()
{
    head.rx() -= SNAKE_SIZE;
    
}

void Snake::moveRight()
{
    head.rx() += SNAKE_SIZE;
    
}

void Snake::moveUp()
{
    head.ry() -= SNAKE_SIZE;
    
}

void Snake::moveDown()
{
    head.ry() += SNAKE_SIZE;
    
}

void Snake::turnEast()
{
  	moveDirection = East;
}

void Snake::turnNortheast()
{
    moveDirection = Northeast;
}

void Snake::turnNorthwest()
{
	moveDirection = Northwest;
}

void Snake::turnSoutheast()
{
    moveDirection = Southeast;
}

void Snake::turnSouthwest()
{
    moveDirection = Southwest;
}

void Snake::turnWest()
{
    moveDirection = West;
}

/**
 * @brief 获取当前移动模式
 * @return 移动模式（四方向或六方向）
 */
MoveMode Snake::getMoveMode() const{ return moveMode; }

/**
 * @brief 处理碰撞检测
 * 检测蛇与食物、墙壁、其他蛇以及自身的碰撞，并通知游戏控制器处理相应事件
 */
void Snake::handleCollisions()
{
    QList<QGraphicsItem *> collisions = collidingItems();

    // Check collisions with other objects on screen
    foreach (QGraphicsItem *collidingItem, collisions) {
        if (collidingItem->data(GD_Type) == GO_Food) {
            // Let GameController handle the event by putting another apple
            controller.snakeAteFood((Food *)collidingItem, this);
            growing += 1;
        } else if (dynamic_cast<Wall*>(collidingItem)){
            controller.snakeHitWall(this, dynamic_cast<Wall*>(collidingItem));
        } else if (dynamic_cast<Snake*>(collidingItem)) {
                controller.snakeHitSnake(this, dynamic_cast<Snake*>(collidingItem));
        }
    }

    // Check snake eating itself
    if (tail.contains(head)) {
        controller.snakeAteItself(this);
    }
}
