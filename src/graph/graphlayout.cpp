#include "graph/graphlayout.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <QHash>
#include <QString>
#include <map>

namespace {
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

class DistanceVisitor : public boost::default_bfs_visitor {
public:
    explicit DistanceVisitor(std::vector<int>& distances) : distancesRef(distances) {}

    template <typename Edge, typename GraphT>
    void tree_edge(Edge e, const GraphT& g) const {
        auto u = boost::source(e, g);
        auto v = boost::target(e, g);
        distancesRef[v] = distancesRef[u] + 1;
    }

private:
    std::vector<int>& distancesRef;
};
}

QHash<QString, int> GraphLayout::computeInstanceLayerDistances(const Info& info) {
    QHash<QString, int> result;
    if (info.instances.isEmpty()) {
        return result;
    }

    Graph g;
    std::map<QString, Vertex> instanceNode;
    QHash<QString, QString> portToInstance;

    for (const auto& inst : info.instances) {
        Vertex v = boost::add_vertex(g);
        instanceNode[inst.id] = v;
        for (const auto& port : inst.ports) {
            portToInstance.insert(port.id, inst.id);
        }
    }

    for (const auto& net : info.nets) {
        for (int i = 0; i + 1 < net.connectedPorts.size(); ++i) {
            const QString fromInst = portToInstance.value(net.connectedPorts[i]);
            const QString toInst = portToInstance.value(net.connectedPorts[i + 1]);
            if (fromInst.isEmpty() || toInst.isEmpty() || fromInst == toInst) {
                continue;
            }
            boost::add_edge(instanceNode[fromInst], instanceNode[toInst], g);
        }
    }

    std::vector<int> distances(boost::num_vertices(g), -1);
    const Vertex root = instanceNode.begin()->second;
    distances[root] = 0;
    DistanceVisitor visitor(distances);
    boost::breadth_first_search(g, root, boost::visitor(visitor));

    for (const auto& inst : info.instances) {
        result.insert(inst.id, distances[instanceNode[inst.id]]);
    }
    return result;
}
