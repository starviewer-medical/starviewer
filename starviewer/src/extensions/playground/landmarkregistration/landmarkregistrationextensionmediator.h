/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLANDMARKREGISTRATIONEXTENSIONMEDIATOR_H
#define UDGLANDMARKREGISTRATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qlandmarkregistrationextension.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LandmarkRegistrationExtensionMediator : public ExtensionMediator
{
public:
    LandmarkRegistrationExtensionMediator(QObject *parent = 0);

    ~LandmarkRegistrationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QLandmarkRegistrationExtension, LandmarkRegistrationExtensionMediator> registerLandmarkRegistrationExtension;

}

#endif
