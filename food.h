#ifndef FOOD_H
#define FOOD_H

#include <QGraphicsItem>

class Food : public QGraphicsItem
{
public:
    /**
     * @brief 构造函数，初始化食物的位置
     * @param x 食物在场景中的x坐标
     * @param y 食物在场景中的y坐标
     */
    Food(qreal x, qreal y);
    
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    QPainterPath shape() const;
    /**
     * @brief 重写QGraphicsItem的shape函数，用于精确的碰撞检测
     * @return 返回食物的形状路径，通常为圆形
     */

};

#endif // FOOD_H
