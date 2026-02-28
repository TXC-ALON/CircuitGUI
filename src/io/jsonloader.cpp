#include "io/jsonloader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
QPointF parsePoint(const QJsonObject& o) {
    return QPointF(o.value("x").toDouble(), o.value("y").toDouble());
}
}

bool JsonLoader::loadFromFile(const QString& filePath, Info& info, QString& error) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        error = QString("Cannot open file: %1").arg(filePath);
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        error = parseError.errorString();
        return false;
    }

    info = Info{};
    const QJsonObject root = doc.object();

    const QJsonArray instances = root.value("instances").toArray();
    for (const QJsonValue& instanceVal : instances) {
        const QJsonObject instanceObj = instanceVal.toObject();
        InstanceInfo inst;
        inst.id = instanceObj.value("id").toString();
        inst.name = instanceObj.value("name").toString();
        inst.module = instanceObj.value("module").toString();
        inst.position = parsePoint(instanceObj.value("position").toObject());

        const QJsonArray ports = instanceObj.value("ports").toArray();
        for (const QJsonValue& portVal : ports) {
            const QJsonObject portObj = portVal.toObject();
            PortInfo port;
            port.id = portObj.value("id").toString();
            port.type = portObj.value("type").toString();
            port.position = parsePoint(portObj.value("position").toObject());
            port.connected = portObj.value("connected").toBool(false);
            inst.ports.push_back(port);
        }
        info.instances.push_back(inst);
    }

    const QJsonArray nets = root.value("nets").toArray();
    for (const QJsonValue& netVal : nets) {
        const QJsonObject netObj = netVal.toObject();
        NetInfo net;
        net.id = netObj.value("id").toString();
        net.status = netObj.value("status").toString();

        const QJsonArray connectedPorts = netObj.value("connected_ports").toArray();
        for (const QJsonValue& idVal : connectedPorts) {
            net.connectedPorts.push_back(idVal.toString());
        }
        info.nets.push_back(net);
    }

    return true;
}
