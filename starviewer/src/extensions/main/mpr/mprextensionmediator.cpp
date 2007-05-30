/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mprextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg{

MPRExtensionMediator::MPRExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


MPRExtensionMediator::~MPRExtensionMediator()
{
}

DisplayableID MPRExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPRExtension",tr("MPR 2D"));
}

bool MPRExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QMPRExtension *mprExtension;

    if ( !(mprExtension = qobject_cast<QMPRExtension*>(extension)) )
    {
        return false;
    }

    VolumeRepository* volumeRepository = VolumeRepository::getRepository();
    mprExtension->setInput(volumeRepository->getVolume( mainVolumeID ));

    return true;
}

} //udg namespace
