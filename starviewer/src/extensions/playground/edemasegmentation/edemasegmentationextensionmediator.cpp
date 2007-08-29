/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "edemasegmentationextensionmediator.h"

#include "extensionhandler.h"
#include "extensioncontext.h"

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

bool EdemaSegmentationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QEdemaSegmentationExtension *edemaSegmentationExtension;

    if ( !(edemaSegmentationExtension = qobject_cast<QEdemaSegmentationExtension*>(extension)) )
    {
        return false;
    }

    edemaSegmentationExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}


}
