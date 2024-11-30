#pragma once

#include <string>

class LAbstractPlugin {
public:
    virtual ~LAbstractPlugin() = default;
    virtual std::string name() = 0;
    virtual std::string description() = 0;
    virtual void run() = 0;
};

#ifdef WIN32
#define LPLUGIN_EXPORT_SYMBOL extern "C" __declspec(dllexport)
#else
#define LPLUGIN_EXPORT_SYMBOL extern "C" __attribute__((visibility("default")))
#endif
