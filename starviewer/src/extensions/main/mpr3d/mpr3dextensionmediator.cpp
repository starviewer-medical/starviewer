/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mpr3dextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg{

MPR3DExtensionMediator::MPR3DExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


MPR3DExtensionMediator::~MPR3DExtensionMediator()
{
}

DisplayableID MPR3DExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPR3DExtension",tr("MPR 3D"));
}

bool MPR3DExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QMPR3DExtension *mpr3dExtension;

    if ( !(mpr3dExtension = qobject_cast<QMPR3DExtension*>(extension)) )
    {
        return false;
    }

    mpr3dExtension->setInput(VolumeRepository::getRepository()->getVolume( extensionContext.getMainVolumeID() ));

    return true;
}

} //udg namespace
