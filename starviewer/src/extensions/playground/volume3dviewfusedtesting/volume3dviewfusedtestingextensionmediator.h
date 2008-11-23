/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef VOLUME3DVIEWFUSEDTESTINGEXTENSIONMEDIATOR_H
#define VOLUME3DVIEWFUSEDTESTINGEXTENSIONMEDIATOR_H

#include "extensionmediator.h"
#include "installextension.h"
#include "qvolume3dviewfusedtestingextension.h"//canviar a fused

namespace udg {

/**
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Volume3DViewFusedTestingExtensionMediator : public ExtensionMediator
{
    Q_OBJECT
public:

    Volume3DViewFusedTestingExtensionMediator( QObject * parent = 0 );
    ~Volume3DViewFusedTestingExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

};

static InstallExtension< QVolume3DViewFusedTestingExtension, Volume3DViewFusedTestingExtensionMediator > registerVolume3DViewFusedTestingExtension; //canviar a fused el Q i no se que fer amb el register???

}

#endif
