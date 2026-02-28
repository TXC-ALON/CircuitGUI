#include "items/portitem.h"

#include <QPainter>

PortItem::PortItem(QString name, QGraphicsItem* parent)
    : QGraphicsItem(parent), portName(std::move(name)) {
    setZValue(2);
}

void PortItem::setState(PortState st) {
    if (portState == st) {
        return;
    }
    portState = st;
    update();
}

PortState PortItem::state() const {
    return portState;
}

void PortItem::setNet(NetItem* net) {
    attachedNet = net;
    setState(net ? PortState::Connected : PortState::Disconnected);
}

NetItem* PortItem::net() const {
    return attachedNet;
}

QString PortItem::id() const {
    return portName;
}

QRectF PortItem::boundingRect() const {
    return QRectF(-6, -6, 12, 12);
}

void PortItem::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::black, 1.5);
    p->setPen(pen);

    if (portState == PortState::Connected) {
        p->setBrush(QColor("#2ecc71"));
        QPolygonF diamond;
        diamond << QPointF(0, -5) << QPointF(5, 0) << QPointF(0, 5) << QPointF(-5, 0);
        p->drawPolygon(diamond);
    } else {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(QColor("#e74c3c"), 2));
        p->drawEllipse(QPointF(0, 0), 5, 5);
    }
}
