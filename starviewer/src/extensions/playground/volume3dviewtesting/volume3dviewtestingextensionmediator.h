/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef VOLUME3DVIEWTESTINGEXTENSIONMEDIATOR_H
#define VOLUME3DVIEWTESTINGEXTENSIONMEDIATOR_H

#include "extensionmediator.h"
#include "installextension.h"
#include "qvolume3dviewtestingextension.h"

namespace udg {

/**
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Volume3DViewTestingExtensionMediator : public ExtensionMediator {

public:

    Volume3DViewTestingExtensionMediator( QObject * parent = 0 );
    ~Volume3DViewTestingExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension( QWidget * extension, ExtensionHandler * extensionHandler, Identifier mainVolumeID );

};

static InstallExtension< QVolume3DViewTestingExtension, Volume3DViewTestingExtensionMediator > registerVolume3DViewTestingExtension;

}

#endif
