#include "proc-timeline.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>

ProcTimeline::ProcTimeline(
    ProcType procType,
    QGraphicsView *parent
) : mProcType(procType)
  , view(parent)
{
    setFlag(ItemSendsGeometryChanges);
}

QRectF
ProcTimeline::boundingRect(void) const
{
    return QRectF(0, 0, 1000, 100);
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
