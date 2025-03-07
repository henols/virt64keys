#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include "IProcessHandler.h"

class WiFiConfig : public IProcessHandler {
public:
    WiFiConfig();
    virtual bool validateHandshake(const std::string &cmd) override;
    virtual ProcessStatus process() override;
    virtual void release() override;
private:
};

#endif // WIFICONFIG_H
