#include "items/netitem.h"
#include "items/portitem.h"

#include <QPainter>

NetItem::NetItem(QGraphicsItem* parent) : QGraphicsPathItem(parent) {
    setPen(QPen(QColor("#34495e"), 2));
    setZValue(1);
}

void NetItem::addPort(PortItem* port) {
    if (!port) {
        return;
    }
    connectedPorts.push_back(port);
    port->setNet(this);
    rebuildPath();
}

QVector<PortItem*> NetItem::ports() const {
    return connectedPorts;
}

void NetItem::rebuildPath() {
    if (connectedPorts.isEmpty()) {
        setPath(QPainterPath());
        return;
    }

    QPainterPath p;
    const QPointF first = connectedPorts.front()->scenePos();
    p.moveTo(first);

    for (int i = 1; i < connectedPorts.size(); ++i) {
        const QPointF prev = connectedPorts[i - 1]->scenePos();
        const QPointF curr = connectedPorts[i]->scenePos();
        const QPointF mid(prev.x(), curr.y());
        p.lineTo(mid);
        p.lineTo(curr);
    }

    setPath(p);
}
