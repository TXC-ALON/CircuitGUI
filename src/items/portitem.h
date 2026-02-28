#pragma once

#include <QGraphicsItem>
#include <QString>

class NetItem;

enum class PortState { Connected, Disconnected };

class PortItem : public QGraphicsItem {
public:
    explicit PortItem(QString name, QGraphicsItem* parent = nullptr);

    void setState(PortState st);
    PortState state() const;

    void setNet(NetItem* net);
    NetItem* net() const;

    QString id() const;

    QRectF boundingRect() const override;
    void paint(QPainter* p, const QStyleOptionGraphicsItem* opt, QWidget* w) override;

private:
    QString portName;
    PortState portState = PortState::Disconnected;
    NetItem* attachedNet = nullptr;
};
