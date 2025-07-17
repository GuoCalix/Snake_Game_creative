#ifndef RECTGRIDITEM_H
#define RECTGRIDITEM_H

#include <QGraphicsItem>
#include <QPainter>

class RectGridItem : public QGraphicsItem
{
public:
    RectGridItem(int rows, int cols, qreal tileSize, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
    int rows;
    int cols;
    qreal tileSize;
};

#endif // RECTGRIDITEM_H