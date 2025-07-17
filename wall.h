#ifndef WALL_H
#define WALL_H

#include <QGraphicsItem>
#include <QPainter>

class Wall : public QGraphicsItem
{
public:
    Wall();
    QRectF boundingRect() const override;
    Wall(int x, int y, int width, int height);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *) override;


private:
    qreal xPos;
    qreal yPos;
    qreal wallWidth;
    qreal wallHeight;
};

#endif // WALL_H
