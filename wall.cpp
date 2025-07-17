#include "wall.h"
#include <QPainter>

/**
 * @brief 墙壁类构造函数
 * @param x 墙壁左上角x坐标
 * @param y 墙壁左上角y坐标
 * @param width 墙壁宽度
 * @param height 墙壁高度
 * 初始化墙壁的位置和尺寸
 */
Wall::Wall(int x, int y, int width, int height)
    : xPos(x),yPos(y),wallWidth(width),wallHeight(height)
{
}

/**
 * @brief 返回墙壁的边界矩形
 * @return 包含整个墙壁的矩形区域
 * 定义墙壁在场景中的位置和大小，用于碰撞检测和绘制范围
 */
QRectF Wall::boundingRect() const
{
    return QRectF(xPos, yPos, wallWidth, wallHeight);
}

/**
 * @brief 绘制墙壁
 * @param painter 绘图工具
 * @param 样式选项（未使用）
 * @param 父窗口部件（未使用）
 * 使用黑色填充墙壁的边界矩形，实现墙壁的可视化
 */
void Wall::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->fillRect(boundingRect(), Qt::black);
    painter->restore();
}
