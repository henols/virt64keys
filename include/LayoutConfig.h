#ifndef LAYOUTCONFIG_H
#define LAYOUTCONFIG_H

#include "IProcessHandler.h"

class LayoutConfig : public IProcessHandler {
public:
    LayoutConfig();
    virtual bool validateHandshake(const std::string &cmd) override;
    virtual ProcessStatus process() override;
    virtual void release() override;
private:
};

#endif // LAYOUTCONFIG_H
