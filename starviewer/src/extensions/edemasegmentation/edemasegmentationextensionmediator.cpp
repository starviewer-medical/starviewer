/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "edemasegmentationextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg {

EdemaSegmentationExtensionMediator::EdemaSegmentationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


EdemaSegmentationExtensionMediator::~EdemaSegmentationExtensionMediator()
{
}

DisplayableID EdemaSegmentationExtensionMediator::getExtensionID() const
{
    return DisplayableID("EdemaSegmentationExtension",tr("Edema Segmentation"));
}

bool EdemaSegmentationExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QEdemaSegmentationExtension *edemaSegmentationExtension;

    if ( !(edemaSegmentationExtension = qobject_cast<QEdemaSegmentationExtension*>(extension)) )
    {
        return false;
    }

    edemaSegmentationExtension->setInput(VolumeRepository::getRepository()->getVolume( mainVolumeID ));

    return true;
}


}
