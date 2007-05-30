/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mpr3dextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

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

bool MPR3DExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QMPR3DExtension *mpr3dExtension;

    if ( !(mpr3dExtension = qobject_cast<QMPR3DExtension*>(extension)) )
    {
        return false;
    }

    VolumeRepository* volumeRepository = VolumeRepository::getRepository();
    mpr3dExtension->setInput(volumeRepository->getVolume( mainVolumeID ));

    return true;
}

} //udg namespace
