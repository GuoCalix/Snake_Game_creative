#ifndef RECTGRIDITEM_H
#define RECTGRIDITEM_H

#include <QGraphicsItem>
#include <QPainter>

/**
 * @brief 矩形网格项类，继承自QGraphicsItem，用于在游戏场景中绘制矩形网格背景
 * 主要在四方向移动模式下作为游戏背景，辅助显示蛇和食物的位置
 */

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
