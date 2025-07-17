#ifndef WALL_H
#define WALL_H

#include <QGraphicsItem>
#include <QPainter>

/**
 * @brief 墙壁类，继承自QGraphicsItem，用于在游戏场景中表示障碍物
 * 蛇碰撞到墙壁会导致游戏结束（单人模式）或判定失败（双人模式）
 */

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
