#pragma once

#include <QPointF>
#include <QString>
#include <QVector>

struct PortInfo {
    QString id;
    QString type;
    QPointF position;
    bool connected = false;
};

struct InstanceInfo {
    QString id;
    QString name;
    QString module;
    QPointF position;
    QVector<PortInfo> ports;
};

struct NetInfo {
    QString id;
    QVector<QString> connectedPorts;
    QString status;
};

struct Info {
    QVector<InstanceInfo> instances;
    QVector<NetInfo> nets;
};
