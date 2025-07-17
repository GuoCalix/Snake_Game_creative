#include "rectgriditem.h"

/**
 * @brief 矩形网格项构造函数
 * @param rows 网格行数
 * @param cols 网格列数
 * @param tileSize 每个网格单元的大小
 * @param parent 父级图形项
 * 初始化矩形网格项，设置行数、列数和单元格大小
 */
RectGridItem::RectGridItem(int rows, int cols, qreal tileSize, QGraphicsItem *parent)
    : QGraphicsItem(parent), rows(rows), cols(cols), tileSize(tileSize) {}

/**
 * @brief 返回包围整个矩形网格的边界矩形
 * @return 边界矩形
 * 计算并返回包含所有网格单元的矩形区域
 */
QRectF RectGridItem::boundingRect() const
{
    return QRectF(0, 0, cols * tileSize, rows * tileSize);
}

/**
 * @brief 绘制矩形网格
 * @param painter 用于绘制的QPainter对象
 * @param option 样式选项（未使用）
 * @param widget 父窗口部件（未使用）
 * 使用灰色画笔绘制所有网格线
 */
void RectGridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::gray);// 设置灰色画笔绘制网格线

    // 遍历所有行和列，绘制每个网格单元
    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            painter->drawRect(col * tileSize, row * tileSize, tileSize, tileSize);
        }
    }
}
