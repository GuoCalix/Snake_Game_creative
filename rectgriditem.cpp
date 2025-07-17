#include "rectgriditem.h"

RectGridItem::RectGridItem(int rows, int cols, qreal tileSize, QGraphicsItem *parent)
    : QGraphicsItem(parent), rows(rows), cols(cols), tileSize(tileSize) {}

QRectF RectGridItem::boundingRect() const
{
    return QRectF(0, 0, cols * tileSize, rows * tileSize);
}

void RectGridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::gray);
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            painter->drawRect(col * tileSize, row * tileSize, tileSize, tileSize);
        }
    }
}