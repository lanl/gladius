#ifndef TIMELINE_PROC_TIMELINE_H_INCLUDED
#define TIMELINE_PROC_TIMELINE_H_INCLUDED

#include "common.h"

#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
class QRectF;
class QGraphicsView;
QT_END_NAMESPACE

class ProcTimeline : public QGraphicsItem {
public:
    //
    ProcTimeline(
        ProcType procType,
        QGraphicsView *parent
    );
    //
    QRectF boundingRect(void) const;
    //
    void
    paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
    );
private:
    ProcType mProcType = ProcType::UNKNOWN;
    //
    QGraphicsView *view = nullptr;
    //
    QRectF mBoundingRect;
};

#endif // TIMELINE_PROC_TIMELINE_H_INCLUDED
