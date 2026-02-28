#pragma once

#include "model/info.h"

#include <QHash>

class GraphLayout {
public:
    // Returns BFS-like distance from first instance as a coarse layering metric.
    static QHash<QString, int> computeInstanceLayerDistances(const Info& info);
};
