#pragma once

#include <QGraphicsRectItem>
#include <QVector>

class PortItem;

class InstanceItem : public QGraphicsRectItem {
public:
    InstanceItem(QString instName, QString moduleName, QGraphicsItem* parent = nullptr);

    void addPort(PortItem* port);
    QVector<PortItem*> ports() const;

    QString getInstName() const;
    QString getModuleName() const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QString instName;
    QString moduleName;
    QVector<PortItem*> portItems;
};
