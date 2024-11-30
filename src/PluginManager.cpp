#include <filesystem>
#include <functional>
#include <iostream>

#include "PluginManager.h"

#ifdef WIN32
#include <Windows.h>
#define EXTENSION ".dll"
#else
#include <dlfcn.h>
#define EXTENSION ".so"
#endif

PluginManager::PluginManager() {
    namespace fs = std::filesystem;

    try {
        for (const auto& entry : fs::directory_iterator("plugins")) {
            if (entry.is_regular_file() && entry.path().extension() == EXTENSION) {
                std::cout << "Found plugin: " << entry.path().string() << std::endl;

#ifdef WIN32
                HMODULE hModule = LoadLibrary(entry.path().string().c_str());
                if (!hModule) {
                    std::cerr << "Failed to load library: " << entry.path().string() << std::endl;
                    continue;
                }

                using GetInstanceFunc = LAbstractPlugin* (*)();
                GetInstanceFunc getInstance =
                    (GetInstanceFunc)GetProcAddress(hModule, "GetInstance");
#else
                void* handle = dlopen(entry.path().string().c_str(), RTLD_LAZY);
                if (!handle) {
                    std::cerr << "Failed to load library: " << entry.path().string() << std::endl;
                    continue;
                }

                using GetInstanceFunc = LAbstractPlugin* (*)();
                GetInstanceFunc getInstance = (GetInstanceFunc)dlsym(handle, "GetInstance");
#endif

                if (!getInstance) {
                    std::cerr << "Failed to get GetInstance function from library: "
                              << entry.path().string() << std::endl;
                    continue;
                }

                LAbstractPlugin* plugin = getInstance();
                if (!plugin) {
                    std::cerr << "Failed to get plugin instance from library: "
                              << entry.path().string() << std::endl;
                    continue;
                }

                std::cout << "Loaded plugin: " << plugin->name() << std::endl;
                m_plugins[plugin->name()] = plugin;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }

    for (const auto& pair : m_plugins) {
        std::cout << "Plugin: " << pair.second->name() << std::endl;
        std::cout << "Description: " << pair.second->description() << std::endl;
        pair.second->run();
    }
}
