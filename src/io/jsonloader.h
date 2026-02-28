#pragma once

#include "model/info.h"

#include <QString>

class JsonLoader {
public:
    static bool loadFromFile(const QString& filePath, Info& info, QString& error);
};
