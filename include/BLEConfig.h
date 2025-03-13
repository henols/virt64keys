#ifndef BLECONFIG_H
#define BLECONFIG_H

#include "IProcessHandler.h"

class BLEConfig : public IProcessHandler {
public:
    BLEConfig();
    virtual ProcessStatus process() override;
    virtual void release() override;
private:
};

#endif // BLECONFIG_H
