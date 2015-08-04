#include "scene.h"

#include <QGraphicsScene>
#include <QRectF>

Scene::Scene(
    QWidget *parent
) : QGraphicsScene(parent)
{
    mSceneRect = QRectF(parent->geometry());
    setSceneRect(mSceneRect);
    addRect(mSceneRect);
}
