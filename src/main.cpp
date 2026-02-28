#include "graph/graphlayout.h"
#include "io/jsonloader.h"
#include "items/instanceitem.h"
#include "items/netitem.h"
#include "items/portitem.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>

namespace {
void populateScene(const Info& info, QGraphicsScene& scene) {
    QHash<QString, PortItem*> portsById;

    for (const auto& inst : info.instances) {
        auto* instanceItem = new InstanceItem(inst.name, inst.module);
        instanceItem->setPos(inst.position);
        scene.addItem(instanceItem);

        for (const auto& port : inst.ports) {
            auto* portItem = new PortItem(port.id);
            portItem->setState(port.connected ? PortState::Connected : PortState::Disconnected);
            portItem->setPos(port.position - inst.position);
            instanceItem->addPort(portItem);
            portsById.insert(port.id, portItem);
        }
    }

    for (const auto& net : info.nets) {
        auto* netItem = new NetItem();
        for (const auto& portId : net.connectedPorts) {
            if (portsById.contains(portId)) {
                netItem->addPort(portsById[portId]);
            }
        }
        scene.addItem(netItem);
    }
}

Info sampleInfo() {
    Info info;
    InstanceInfo a;
    a.id = "inst_a";
    a.name = "U1";
    a.module = "ALU";
    a.position = QPointF(80, 120);
    a.ports.push_back(PortInfo{"p1", "output", QPointF(80, 140), true});

    InstanceInfo b;
    b.id = "inst_b";
    b.name = "U2";
    b.module = "REG";
    b.position = QPointF(360, 220);
    b.ports.push_back(PortInfo{"p2", "input", QPointF(360, 240), true});

    info.instances = {a, b};
    info.nets.push_back(NetInfo{"n1", {"p1", "p2"}, "active"});
    return info;
}
} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Info info;
    QString error;
    if (argc > 1) {
        if (!JsonLoader::loadFromFile(argv[1], info, error)) {
            info = sampleInfo();
        }
    } else {
        info = sampleInfo();
    }

    (void)GraphLayout::computeInstanceLayerDistances(info);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 900, 600);
    populateScene(info, scene);

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing, true);
    view.setWindowTitle("CircuitGUI Prototype");
    view.resize(1000, 700);
    view.show();

    return app.exec();
}
