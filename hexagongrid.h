#ifndef HEXAGONGRID_H
#define HEXAGONGRID_H

#include <QGraphicsItem>
#include <QVector>
#include <QPointF>
#include <QPainterPath>

/**
 * @brief 六边形网格类，用于在游戏场景中绘制六边形网格背景
 * 该类继承自QGraphicsItem，实现了六边形网格的生成和渲染
 */

class HexagonGrid : public QGraphicsItem
{
public:
    /**
     * @brief 构造函数，初始化六边形网格
     * @param tileSize 单个六边形的大小
     * @param windowWidth 窗口宽度，用于计算网格数量
     * @param windowHeight 窗口高度，用于计算网格数量
     * @param parent 父级图形项
     */
    HexagonGrid(qreal tileSize, qreal windowWidth, qreal windowHeight, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    qreal tileSize; // 单个六边形的大小
    int rows;   // 网格行数
    int cols;   // 网格列数
    QVector<QVector<QPointF>> hexagonCenters;

    QPainterPath createHexagonPath(const QPointF &center) const;
};

#endif // HEXAGONGRID_H
