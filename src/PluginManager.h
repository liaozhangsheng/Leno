#pragma once

#include <string>
#include <unordered_map>

#include "PluginInterface.h"

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

private:
    std::unordered_map<std::string, LAbstractPlugin *> m_plugins;
};
