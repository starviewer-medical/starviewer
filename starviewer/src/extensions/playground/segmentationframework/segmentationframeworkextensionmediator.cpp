/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "segmentationframeworkextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

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

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        segmentationFrameworkExtension->setInput( input );

    return true;
}

}
