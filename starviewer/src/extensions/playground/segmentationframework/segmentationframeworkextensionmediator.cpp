/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "segmentationframeworkextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

namespace udg {

SegmentationFrameworkExtensionMediator::SegmentationFrameworkExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

SegmentationFrameworkExtensionMediator::~SegmentationFrameworkExtensionMediator()
{
}

DisplayableID SegmentationFrameworkExtensionMediator::getExtensionID() const
{
    return DisplayableID("SegmentationFrameworkExtension",tr("Segmentation Framework"));
}

bool SegmentationFrameworkExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QSegmentationFrameworkExtension *segmentationFrameworkExtension;

    if ( !(segmentationFrameworkExtension = qobject_cast<QSegmentationFrameworkExtension*>(extension)) )
    {
        return false;
    }

    segmentationFrameworkExtension->setInput(VolumeRepository::getRepository()->getVolume( mainVolumeID ));

    return true;
}

}
