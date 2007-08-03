/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mpr3d2dextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg{

MPR3D2DExtensionMediator::MPR3D2DExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


MPR3D2DExtensionMediator::~MPR3D2DExtensionMediator()
{
}

DisplayableID MPR3D2DExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPR3D2DExtension",tr("MPR 3D-2D"));
}

bool MPR3D2DExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QMPR3D2DExtension *mpr3d2dExtension;

    if ( !(mpr3d2dExtension = qobject_cast<QMPR3D2DExtension*>(extension)) )
    {
        return false;
    }

    mpr3d2dExtension->setInput(VolumeRepository::getRepository()->getVolume( extensionContext.getMainVolumeID() ));

    return true;
}

} //udg namespace
