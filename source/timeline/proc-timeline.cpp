#include "proc-timeline.h"

#include <QBrush>
#include <QDebug>

ProcTimeline::ProcTimeline(
    QGraphicsView *parent
) : view(parent)
{
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

QRectF
ProcTimeline::boundingRect(void) const
{
    return QRectF(0, 0, 2000, 200);
}

void
ProcTimeline::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget
) {
    QBrush brush(Qt::blue);
    painter->setBrush(brush);
    for (int i = 0; i < 100; ++i) {
        painter->drawRect(QRectF(i * 10, 0, 20, 20));
    }
}
