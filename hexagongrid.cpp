#include "hexagongrid.h"
#include <QPainter>
#include <cmath>
#include <limits>

/**
 * @brief 六边形网格构造函数
 * @param tileSize 单个六边形的大小
 * @param windowWidth 窗口宽度
 * @param windowHeight 窗口高度
 * @param parent 父级图形项
 * 初始化六边形网格，计算网格的列数和行数，并生成所有六边形中心坐标
 */
HexagonGrid::HexagonGrid(qreal tileSize, qreal windowWidth, qreal windowHeight, QGraphicsItem *parent)
    : QGraphicsItem(parent), tileSize(tileSize)
{
    // 计算六边形网格的水平和垂直间距
    const qreal dx = std::sqrt(3) * tileSize ; // 水平间距
    const qreal dy = 1.5 * tileSize ;          // 垂直间距

    // 根据窗口尺寸和间距计算网格的列数和行数，并增加额外空间
    cols = static_cast<int>(windowWidth / dx) + 50;
    rows = static_cast<int>(windowHeight / dy) + 50;

    // 生成六边形网格的中心坐标
    for (int row = -51; row < rows; ++row) {
        QVector<QPointF> rowCenters;
        for (int col = -50; col < cols; ++col) {
            // 计算每个六边形的中心坐标（交错排列）
            qreal x = (col + 0.5 * (row % 2)) * dx;
            qreal y = row * dy;
            rowCenters.append(QPointF(x, y));
        }
        hexagonCenters.append(rowCenters);
    }
}

/**
 * @brief 返回包围整个六边形网格的矩形区域
 * @return 边界矩形
 * 计算并返回包含所有六边形的最小矩形区域，包含额外的边距
 */
QRectF HexagonGrid::boundingRect() const
{
    if (hexagonCenters.isEmpty() || hexagonCenters.first().isEmpty()) {
        return QRectF();
    }

    // 初始化边界值为最大和最小可能值
    qreal minX = std::numeric_limits<qreal>::max();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    // 遍历所有六边形中心，找到最边界的坐标值
    for (const auto &row : hexagonCenters) {
        for (const auto &center : row) {
            minX = qMin(minX, center.x());
            minY = qMin(minY, center.y());
            maxX = qMax(maxX, center.x());
            maxY = qMax(maxY, center.y());
        }
    }

    // 添加额外边距，确保所有六边形都被包含
    qreal padding = tileSize * 2;
    return QRectF(minX - padding, minY - padding, (maxX - minX) + 2 * padding, (maxY - minY) + 2 * padding);
}

/**
 * @brief 绘制六边形网格
 * @param painter 用于绘制的QPainter对象
 * @param option 样式选项（未使用）
 * @param widget 父窗口部件（未使用）
 * 使用指定的画笔和画刷绘制所有六边形
 */
void HexagonGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // 设置画笔和画刷属性
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::black);

    // 遍历所有六边形中心并绘制
    for (const auto &row : hexagonCenters) {
        for (const auto &center : row) {
            QPainterPath path = createHexagonPath(center);
            painter->drawPath(path);
        }
    }
}

/**
 * @brief 创建单个六边形的路径
 * @param center 六边形的中心坐标
 * @return 六边形的路径对象
 * 根据给定的中心坐标和六边形大小创建六边形的路径
 */
QPainterPath HexagonGrid::createHexagonPath(const QPointF &center) const
{
    QPainterPath path;
    const int sides = 6;
    const qreal radius = tileSize;

    // 计算六边形的六个顶点坐标并连接成路径
    for (int i = 0; i < sides; ++i) {
        // 计算每个顶点的角度（30度旋转，使六边形顶部为尖顶）
        qreal angle_deg = 60 * i + 30; // 30°旋转：尖顶
        qreal angle_rad = angle_deg * M_PI / 180.0;
        qreal x = center.x() + radius * std::cos(angle_rad);
        qreal y = center.y() + radius * std::sin(angle_rad);
        if (i == 0)
            path.moveTo(x, y);// 移动到第一个顶点
        else
            path.lineTo(x, y);// 连接后续顶点
    }

    path.closeSubpath();// 闭合路径，完成六边形
    return path;
}
