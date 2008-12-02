#ifndef UDGGPUTESTINGEXTENSIONMEDIATOR_H
#define UDGGPUTESTINGEXTENSIONMEDIATOR_H


#include "extensionmediator.h"

#include "installextension.h"
#include "qgputestingextension.h"


namespace udg {


class GpuTestingExtensionMediator : public ExtensionMediator {

    Q_OBJECT

public:

    GpuTestingExtensionMediator( QObject *parent = 0 );
    ~GpuTestingExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension( QWidget *extension, const ExtensionContext &extensionContext );

};


static InstallExtension<QGpuTestingExtension, GpuTestingExtensionMediator> registerGpuTestingExtension;


}


#endif
