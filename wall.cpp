#include "wall.h"
#include <QPainter>

Wall::Wall(int x, int y, int width, int height)
    : xPos(x),yPos(y),wallWidth(width),wallHeight(height)
{
}

QRectF Wall::boundingRect() const
{
    return QRectF(xPos, yPos, wallWidth, wallHeight);
}

void Wall::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->fillRect(boundingRect(), Qt::black);
    painter->restore();
}
