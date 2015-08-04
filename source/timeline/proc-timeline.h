#ifndef TIMELINE_PROC_TIMELINE_H_INCLUDED
#define TIMELINE_PROC_TIMELINE_H_INCLUDED

#include <QRectF>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>

class ProcTimeline : public QGraphicsItem {
public:
    //
    ProcTimeline(QGraphicsView *parent);
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
    //
    QGraphicsView *view = nullptr;
    //
    QRectF mBoundingRect;
};

#endif // TIMELINE_PROC_TIMELINE_H_INCLUDED
