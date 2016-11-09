#include "mapview.h"

#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>

MapView::MapView(QWidget *parent, Map *map) : QWidget(parent)
{
    this->map = map;
    showGrid = true;
    gridSize = 64;

    centerX = 1600;
    centerY = 1600;

    setZoom(2);
}

MapView::~MapView()
{

}

void MapView::viewAreaResized(QResizeEvent* evt)
{
    float wdiff = evt->size().width() - evt->oldSize().width();
    float hdiff = evt->size().height() - evt->oldSize().height();

    QPoint wrongPos = getCurrentPos();
    emit scrollTo(QPoint(wrongPos.x() - wdiff/(2*zoom), wrongPos.y() - hdiff/(2*zoom)));
}

void MapView::setZoom(float zoom)
{
    QPoint currPos = getCurrentPos();

    this->zoom = zoom;

    setMinimumSize(centerX*2*zoom, centerY*2*zoom);
    setMaximumSize(centerX*2*zoom, centerY*2*zoom);
    resize(centerX*2*zoom, centerY*2*zoom);

    emit scrollTo(currPos);

    repaint();
}

void MapView::zoomIn()
{
    if (zoom < 3.5)
        setZoom(zoom+0.25);
}

void MapView::zoomOut()
{
    if (zoom > 0.75)
        setZoom(zoom-0.25);
}

void MapView::zoomMax()
{
    setZoom(3.5);
}

void MapView::zoomMin()
{
    setZoom(1);
}

void MapView::setMap(Map* map)
{
    this->map = map;
    update();
}

QPoint MapView::getCurrentPos()
{
    return QPoint((visibleRegion().boundingRect().x() + visibleRegion().boundingRect().width()/2)/zoom - centerX, (visibleRegion().boundingRect().y() + visibleRegion().boundingRect().height()/2)/zoom - centerY);
}

void MapView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.scale(zoom,zoom);

    QRect drawrect = QRect(evt->rect().x()/zoom, evt->rect().y()/zoom, evt->rect().width()/zoom+20, evt->rect().height()/zoom+20);

    painter.fillRect(drawrect, QColor(119,136,153));

    if (showGrid)
    {
        QPen lineBold(Qt::white);
        lineBold.setWidthF(3/zoom);

        painter.setPen(lineBold);

        painter.drawLine(drawrect.x(), centerY, drawrect.x() + drawrect.width(), centerY);
        painter.drawLine(centerX, drawrect.y(), centerX, drawrect.y() + drawrect.height());

        QPen line(Qt::white);
        line.setWidthF(1/zoom);
        painter.setPen(line);

        int y = roundDown(drawrect.y() - centerY, gridSize) + centerY;
        while (y < drawrect.y() + drawrect.height())
        {
            y += gridSize;
            if (y != centerY)
                painter.drawLine(drawrect.x(), y, drawrect.x() + drawrect.width(), y);
        }

        int x = roundDown(drawrect.x() - centerX, gridSize) + centerX;
        while (x < drawrect.x() + drawrect.width())
        {
            x += gridSize;
            if (x != centerX)
                painter.drawLine(x, drawrect.y(), x, drawrect.y() + drawrect.height());
        }
    }

    painter.translate(centerX, centerY);


    // Draw Nodes

    QString nodesPath(QCoreApplication::applicationDirPath() + "/goombatlas_data/nodes/");

    foreach (Node* n, map->nodes)
    {
        QRect nodeRect(n->getx()-10, n->getz()-10, 20, 20);

        QPixmap nodePixmap;
        if (n->getIconId() == 0)
            nodePixmap = QPixmap(nodesPath + "start.png");
        else
            nodePixmap = QPixmap(nodesPath + "normal.png");

        /*if (!n->getSetting(3))
        {
            Draw some different node if node not visible?
        }*/

        painter.drawPixmap(nodeRect, nodePixmap);
    }
}

int MapView::roundDown(int num, int factor)
{
    return num - (num % factor) - factor;
}
