/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumecontourdelimiterextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg{

VolumeContourDelimiterExtensionMediator::VolumeContourDelimiterExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

VolumeContourDelimiterExtensionMediator::~VolumeContourDelimiterExtensionMediator()
{
}

DisplayableID VolumeContourDelimiterExtensionMediator::getExtensionID() const
{
    return DisplayableID("VolumeContourDelimiterExtension",tr("Volume Contour Delimiter"));
}

bool VolumeContourDelimiterExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QVolumeContourDelimiterExtension *volumeContourDelimiterExtension;

    if ( !(volumeContourDelimiterExtension = qobject_cast<QVolumeContourDelimiterExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        volumeContourDelimiterExtension->setInput( input );

    return true;
}

} //udg namespace
