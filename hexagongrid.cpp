#include "hexagongrid.h"
#include <QPainter>
#include <cmath>
#include <limits>

HexagonGrid::HexagonGrid(qreal tileSize, qreal windowWidth, qreal windowHeight, QGraphicsItem *parent)
    : QGraphicsItem(parent), tileSize(tileSize)
{
    const qreal dx = std::sqrt(3) * tileSize ; // 水平间距
    const qreal dy = 1.5 * tileSize ;          // 垂直间距

    cols = static_cast<int>(windowWidth / dx) + 50;
    rows = static_cast<int>(windowHeight / dy) + 50;

    for (int row = -51; row < rows; ++row) {
        QVector<QPointF> rowCenters;
        for (int col = -50; col < cols; ++col) {
            qreal x = (col + 0.5 * (row % 2)) * dx;
            qreal y = row * dy;
            rowCenters.append(QPointF(x, y));
        }
        hexagonCenters.append(rowCenters);
    }
}

QRectF HexagonGrid::boundingRect() const
{
    if (hexagonCenters.isEmpty() || hexagonCenters.first().isEmpty()) {
        return QRectF();
    }

    qreal minX = std::numeric_limits<qreal>::max();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    for (const auto &row : hexagonCenters) {
        for (const auto &center : row) {
            minX = qMin(minX, center.x());
            minY = qMin(minY, center.y());
            maxX = qMax(maxX, center.x());
            maxY = qMax(maxY, center.y());
        }
    }

    qreal padding = tileSize * 2;
    return QRectF(minX - padding, minY - padding, (maxX - minX) + 2 * padding, (maxY - minY) + 2 * padding);
}

void HexagonGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::black);

    for (const auto &row : hexagonCenters) {
        for (const auto &center : row) {
            QPainterPath path = createHexagonPath(center);
            painter->drawPath(path);
        }
    }
}

QPainterPath HexagonGrid::createHexagonPath(const QPointF &center) const
{
    QPainterPath path;
    const int sides = 6;
    const qreal radius = tileSize;

    for (int i = 0; i < sides; ++i) {
        qreal angle_deg = 60 * i + 30; // 30°旋转：尖顶
        qreal angle_rad = angle_deg * M_PI / 180.0;
        qreal x = center.x() + radius * std::cos(angle_rad);
        qreal y = center.y() + radius * std::sin(angle_rad);
        if (i == 0)
            path.moveTo(x, y);
        else
            path.lineTo(x, y);
    }

    path.closeSubpath();
    return path;
}
