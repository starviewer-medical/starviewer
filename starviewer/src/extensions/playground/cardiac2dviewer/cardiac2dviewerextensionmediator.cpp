/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cardiac2dviewerextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

Cardiac2DViewerExtensionMediator::Cardiac2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Cardiac2DViewerExtensionMediator::~Cardiac2DViewerExtensionMediator()
{
}

DisplayableID Cardiac2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Cardiac2dViewerExtension",tr("2D Cardiac Viewer"));
}

bool Cardiac2DViewerExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QCardiac2DViewerExtension *cardiac2DViewerExtension;

    if ( !(cardiac2DViewerExtension = qobject_cast<QCardiac2DViewerExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        cardiac2DViewerExtension->setInput( input );

    return true;
}

}
