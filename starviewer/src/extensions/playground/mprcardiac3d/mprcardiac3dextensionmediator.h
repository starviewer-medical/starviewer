/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMPRCARDIAC3DEXTENSIONMEDIATOR_H
#define UDGMPRCARDIAC3DEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmprcardiac3dextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPRCardiac3DExtensionMediator : public ExtensionMediator
{
public:
    MPRCardiac3DExtensionMediator(QObject *parent = 0);

    ~MPRCardiac3DExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPRCardiac3DExtension, MPRCardiac3DExtensionMediator> registerMPRCardiac3DExtension;

}

#endif
