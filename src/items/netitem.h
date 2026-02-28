#pragma once

#include <QGraphicsPathItem>
#include <QVector>

class PortItem;

class NetItem : public QGraphicsPathItem {
public:
    explicit NetItem(QGraphicsItem* parent = nullptr);

    void addPort(PortItem* port);
    QVector<PortItem*> ports() const;
    void rebuildPath();

private:
    QVector<PortItem*> connectedPorts;
};
