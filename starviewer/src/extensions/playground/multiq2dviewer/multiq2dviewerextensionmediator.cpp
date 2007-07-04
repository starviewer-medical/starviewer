/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "multiq2dviewerextensionmediator.h"
#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg{

MultiQ2DViewerExtensionMediator::MultiQ2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


MultiQ2DViewerExtensionMediator::~MultiQ2DViewerExtensionMediator()
{
}

DisplayableID MultiQ2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("MultiQ2DViewerExtension",tr("MultiQ2DViewer"));
}

bool MultiQ2DViewerExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    MultiQ2DViewerExtension * multiq2dviewerExtension;

    if ( !(multiq2dviewerExtension = qobject_cast<MultiQ2DViewerExtension*>(extension)) )
    {
        return false;
    }

    VolumeRepository* volumeRepository = VolumeRepository::getRepository();
    multiq2dviewerExtension->setInput(volumeRepository->getVolume( mainVolumeID ));

    return true;
}

} //udg namespace
