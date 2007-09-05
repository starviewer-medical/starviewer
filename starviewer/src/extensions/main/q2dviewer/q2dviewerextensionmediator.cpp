/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextensionmediator.h"

#include "extensioncontext.h"

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
    q2dviewerExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

} //udg namespace
