#pragma once
#include <string>

template<typename TTCmd>
class ICMDNotifyFunc
{
public:
    virtual ~ICMDNotifyFunc() {}

    virtual bool notify(const std::string& actionName, const TTCmd* pcmd) const = 0;
};