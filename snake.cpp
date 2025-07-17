#include <QPainter>

#include "constants.h"
#include "gamecontroller.h"
#include "snake.h"
#include "wall.h"

static const qreal SNAKE_SIZE = TILE_SIZE;

Snake::Snake(GameController &controller, int player, MoveMode movemode) :
 	head(0,0),
    growing(1),
    speed(5),
    moveDirection(NoMove),
    controller(controller),
    player(player),
    moveMode(movemode)
{

  	setZValue(1);

    if (player == 1) {
        head = QPointF(500, -500).toPoint();  // 对齐到网格
    } else if (player == 2) {
        head = QPointF(-500, 500).toPoint();
    }
    // 确保坐标是TILE_SIZE的整数倍
    head.rx() = qRound(head.x() / 40) * 40;
    head.ry() = qRound(head.y() / 40) * 40;

    setPos(head);
    tail.clear();

}

QRectF Snake::boundingRect() const
{
    qreal minX = head.x();
    qreal minY = head.y();
    qreal maxX = head.x();
    qreal maxY = head.y();

    foreach (QPointF p, tail) {
        maxX = p.x() > maxX ? p.x() : maxX;
        maxY = p.y() > maxY ? p.y() : maxY;
        minX = p.x() < minX ? p.x() : minX;
        minY = p.y() < minY ? p.y() : minY;
    }

    QPointF tl = mapFromScene(QPointF(minX, minY));
    QPointF br = mapFromScene(QPointF(maxX, maxY));

    QRectF bound = QRectF(tl.x(),  // x
                          tl.y(),  // y
                          br.x() - tl.x() + SNAKE_SIZE,      // width
                          br.y() - tl.y() + SNAKE_SIZE       //height
                          );
    return bound;
}

QPainterPath Snake::shape() const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    path.addRect(QRectF(0, 0, SNAKE_SIZE, SNAKE_SIZE));

    foreach (QPointF p, tail) {
        QPointF itemp = mapFromScene(p);
        path.addRect(QRectF(itemp.x(), itemp.y(), SNAKE_SIZE, SNAKE_SIZE));
    }

    return path;
}

void Snake::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    if (player == 1) {
        painter->fillPath(shape(), Qt::yellow);
    } else {
        painter->fillPath(shape(), Qt::blue);
    }
    painter->restore();
}

void Snake::setMoveDirection(Direction direction)
{
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

Snake::Direction Snake::currentDirection()
{
	return moveDirection;
}

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

MoveMode Snake::getMoveMode() const{ return moveMode; }

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
