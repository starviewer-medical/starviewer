/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "segmentationframeworkextensionmediator.h"

#include "extensioncontext.h"

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

bool SegmentationFrameworkExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QSegmentationFrameworkExtension *segmentationFrameworkExtension;

    if ( !(segmentationFrameworkExtension = qobject_cast<QSegmentationFrameworkExtension*>(extension)) )
    {
        return false;
    }

    segmentationFrameworkExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

}
