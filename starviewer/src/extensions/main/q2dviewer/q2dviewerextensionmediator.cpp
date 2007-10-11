/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextensionmediator.h"

#include "extensioncontext.h"
// QT
#include <QMessageBox>

namespace udg{

Q2DViewerExtensionMediator::Q2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Q2DViewerExtensionMediator::~Q2DViewerExtensionMediator()
{
}

DisplayableID Q2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q2DViewerExtension",tr("2D Viewer"));
}

bool Q2DViewerExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    Q2DViewerExtension *q2dviewerExtension;

    if ( !(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)) )
    {
        return false;
    }

    q2dviewerExtension->setPatient( extensionContext.getPatient() );
    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        q2dviewerExtension->setInput( input );

    return true;
}

} //udg namespace
