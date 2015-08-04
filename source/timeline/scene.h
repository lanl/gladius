#ifndef TIMELINE_SCENE_H_INCLUDED
#define TIMELINE_SCENE_H_INCLUDED

#include <QWidget>
#include <QGraphicsScene>
#include <QRectF>

class Scene : public QGraphicsScene {
public:
    //
    Scene(QWidget *parent = nullptr);

private:
    //
    QRectF mSceneRect;
};

#endif // TIMELINE_SCENE_H_INCLUDED
