#ifndef HEXAGONGRID_H
#define HEXAGONGRID_H

#include <QGraphicsItem>
#include <QVector>
#include <QPointF>
#include <QPainterPath>

class HexagonGrid : public QGraphicsItem
{
public:
    HexagonGrid(qreal tileSize, qreal windowWidth, qreal windowHeight, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    qreal tileSize;
    int rows;
    int cols;
    QVector<QVector<QPointF>> hexagonCenters;

    QPainterPath createHexagonPath(const QPointF &center) const;
};

#endif // HEXAGONGRID_H