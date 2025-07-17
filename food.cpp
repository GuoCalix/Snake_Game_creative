#include <QPainter>

#include "constants.h"
#include "food.h"

// 食物半径常量（单位：像素）
static const qreal FOOD_RADIUS = 20;

/**
 * @brief Food类构造函数
 * @param x 食物位置的x坐标
 * @param y 食物位置的y坐标
 * 初始化食物对象，设置位置并标记对象类型为食物
 */
Food::Food(qreal x, qreal y)
{
    setPos(x, y);// 设置食物在场景中的位置
    setData(GD_Type, GO_Food);// 设置元数据，标记此对象为食物类型
}


/**
 * @brief 返回食物的边界矩形
 * @return QRectF 边界矩形对象
 * 定义食物对象在场景中所占的矩形区域（用于碰撞检测）
 */
QRectF Food::boundingRect() const
{
    // 返回边界矩形，以(-TILE_SIZE, -TILE_SIZE)为左上角，宽高为TILE_SIZE*2
    return QRectF(-TILE_SIZE,    -TILE_SIZE,
                   TILE_SIZE * 2, TILE_SIZE * 2 );
}


/**
 * @brief 绘制食物
 * @param painter 绘制器对象
 * @param option 样式选项（未使用）
 * @param widget 父窗口部件（未使用）
 * 使用抗锯齿渲染并以红色填充食物形状
 */
void Food::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();// 保存当前绘制状态

    painter->setRenderHint(QPainter::Antialiasing); // 启用抗锯齿，使图形边缘更平滑
    painter->fillPath(shape(), Qt::red);// 使用红色填充食物形状

    painter->restore();// 恢复之前的绘制状态
}


/**
 * @brief 返回食物的形状路径
 * @return QPainterPath 形状路径对象
 * 定义食物的精确形状（圆形），用于精确碰撞检测
 */
QPainterPath Food::shape() const
{
    QPainterPath p;
    // 添加一个椭圆，圆心位于(TILE_SIZE/2, TILE_SIZE/2)，半径为FOOD_RADIUS
    p.addEllipse(QPointF(TILE_SIZE / 2, TILE_SIZE / 2), FOOD_RADIUS, FOOD_RADIUS);
    return p;
}
