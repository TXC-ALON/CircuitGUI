#include "items/instanceitem.h"
#include "items/portitem.h"

#include <QPainter>

InstanceItem::InstanceItem(QString instName, QString moduleName, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), instName(std::move(instName)), moduleName(std::move(moduleName)) {
    setRect(0, 0, 160, 80);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void InstanceItem::addPort(PortItem* port) {
    if (!port) {
        return;
    }
    port->setParentItem(this);
    const qreal y = 20.0 + (portItems.size() * 16.0);
    port->setPos(rect().left(), y);
    portItems.push_back(port);
}

QVector<PortItem*> InstanceItem::ports() const {
    return portItems;
}

QString InstanceItem::getInstName() const {
    return instName;
}

QString InstanceItem::getModuleName() const {
    return moduleName;
}

void InstanceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QGraphicsRectItem::paint(painter, option, widget);

    painter->setPen(QPen(Qt::darkBlue, 2));
    painter->drawRect(rect());

    painter->setPen(Qt::black);
    painter->drawText(QPointF(rect().left(), rect().top() - 8), instName);
    painter->drawText(QPointF(rect().left(), rect().bottom() + 16), moduleName);
}
